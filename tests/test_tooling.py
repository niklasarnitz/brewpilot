import os
from pathlib import Path
import shutil
import subprocess
import tempfile
import unittest


ROOT = Path(__file__).resolve().parents[1]
BUILD_SCRIPT = ROOT / "tools" / "build-firmware.sh"


class BuildFirmwareTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.directory = Path(self.temp.name)
        self.arguments = self.directory / "arguments"
        self.pio = self.directory / "pio"
        (self.directory / "dirname").symlink_to(shutil.which("dirname"))

    def run_build(self, *arguments):
        env = os.environ.copy()
        env.update(PATH=str(self.directory), PIO_ARGUMENTS=str(self.arguments))
        return subprocess.run(
            [str(BUILD_SCRIPT), *arguments], cwd=self.directory, env=env,
            text=True, capture_output=True,
        )

    def install_pio(self, exit_code=0):
        self.pio.write_text(
            '#!/bin/sh\nprintf "%s\\n" "$@" > "$PIO_ARGUMENTS"\n'
            f'exit {exit_code}\n'
        )
        self.pio.chmod(0o755)

    def test_builds_both_environments_without_upload(self):
        self.install_pio()
        result = self.run_build()
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertEqual(self.arguments.read_text().splitlines(), [
            "run", "--project-dir", str(ROOT),
            "--environment", "rancilio-s27",
            "--environment", "la-cimbali-m29-select",
        ])

    def test_rejects_arguments_before_calling_pio(self):
        self.install_pio()
        for argument in ("--help", "upload", "rancilio-s27"):
            with self.subTest(argument=argument):
                result = self.run_build(argument)
                self.assertEqual(result.returncode, 2)
                self.assertIn("Usage:", result.stderr)
                self.assertFalse(self.arguments.exists())

    def test_missing_pio_is_reported(self):
        result = self.run_build()
        self.assertEqual(result.returncode, 127)
        self.assertIn("PlatformIO", result.stderr)

    def test_pio_failure_is_propagated(self):
        self.install_pio(exit_code=42)
        self.assertEqual(self.run_build().returncode, 42)


class ConfigurationTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.compiler = shutil.which("c++")
        if cls.compiler is None:
            raise unittest.SkipTest("host C++ compiler not found")

    def compile_config(self, source, *definitions):
        return subprocess.run(
            [self.compiler, "-std=c++17", "-fsyntax-only", "-x", "c++",
             "-I", str(ROOT / "src"), *[f"-D{item}" for item in definitions], "-"],
            input=source, text=True, capture_output=True,
        )

    def test_machine_selection(self):
        for macro, driver in (
            ("BREWPILOT_MACHINE_RANCILIO_S27", "RancilioS27ButtonMatrixHandler"),
            ("BREWPILOT_MACHINE_LA_CIMBALI_M29_SELECT", "LaCimbaliM29ButtonMatrixHandler"),
        ):
            with self.subTest(machine=macro):
                result = self.compile_config(
                    '#include <type_traits>\n#include "input/matrix/ButtonMatrix.h"\n'
                    f'static_assert(std::is_same_v<ButtonMatrix, {driver}>, "wrong machine");\n',
                    macro,
                )
                self.assertEqual(result.returncode, 0, result.stderr)

    def test_machine_selection_requires_exactly_one(self):
        for definitions in ((), ("BREWPILOT_MACHINE_RANCILIO_S27",
                                 "BREWPILOT_MACHINE_LA_CIMBALI_M29_SELECT")):
            with self.subTest(definitions=definitions):
                result = self.compile_config('#include "config/SelectedMachine.h"\n', *definitions)
                self.assertNotEqual(result.returncode, 0)
                self.assertIn("Select exactly one BrewPilot machine", result.stderr)

    def test_machine_settings_come_only_from_the_machine_config(self):
        for setting in ("BREWPILOT_GROUP_ONE_ACTIVE=1", "BREWPILOT_GROUP_TWO_ACTIVE=1"):
            with self.subTest(setting=setting):
                result = self.compile_config('#include "config/SelectedMachine.h"\n',
                                             "BREWPILOT_MACHINE_RANCILIO_S27", setting)
                self.assertNotEqual(result.returncode, 0)
                self.assertIn("not with -D", result.stderr)

    def test_a_machine_config_is_only_used_by_its_own_build(self):
        result = self.compile_config('#include "config/machines/LaCimbaliM29Select.h"\n',
                                     "BREWPILOT_MACHINE_RANCILIO_S27")
        self.assertNotEqual(result.returncode, 0)
        self.assertIn("belongs to another build", result.stderr)

if __name__ == "__main__":
    unittest.main()
