"""Public CLI regressions for LLVM's two switch-default counter orientations."""

import json
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
BOILER = ROOT / "src/hardware/state-handlers/BoilerStateHandler.h"
SWITCH_LINE = BOILER.read_text().splitlines().index("            switch (internalState)") + 1


class CoverageGateTests(unittest.TestCase):
    def report(self, counters):
        headers = [BOILER, *(
            ROOT / "src/hardware/state-handlers" / name
            for name in ("GroupHeadHandler.h", "StateHandler.h", "TeaStateHandler.h")
        ), ROOT / "src/hardware/actors/StateActor.h"]
        files = [{
            "filename": str(header),
            "summary": {metric: {"count": 1, "covered": 1}
                        for metric in ("lines", "functions", "regions")},
            "branches": [],
        } for header in headers]
        for file in files:
            file["summary"]["branches"] = {"count": 0, "covered": 0}
        # Minimized from GitHub LLVM 18: [99, 21, 99, 34, 0, 162, 0, 0, 4].
        # Newer local LLVM reports the same location with [162, 0] counters.
        files[0]["branches"] = [[SWITCH_LINE, 21, SWITCH_LINE, 34, *counters, 0, 0, 4]]
        files[0]["summary"]["branches"] = {"count": 2, "covered": sum(count > 0 for count in counters)}
        return {"data": [{"files": files}]}

    def check(self, report):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "coverage.json"
            path.write_text(json.dumps(report))
            return subprocess.run(
                [sys.executable, str(ROOT / "tools/check-state-coverage.py"), str(path)],
                capture_output=True, text=True, check=False,
            )

    def test_accepts_both_llvm_default_counter_orders(self):
        for counters in ([0, 162], [162, 0]):
            with self.subTest(counters=counters):
                result = self.check(self.report(counters))
                self.assertEqual(result.returncode, 0, result.stderr)
                self.assertIn("UNREACHABLE (not covered)", result.stdout)

    def test_rejects_another_uncovered_branch(self):
        for counters in ([0, 162], [162, 0]):
            with self.subTest(counters=counters):
                report = self.report(counters)
                boiler = report["data"][0]["files"][0]
                boiler["branches"].append([102, 21, 102, 36, 0, 12, 0, 0, 4])
                boiler["summary"]["branches"] = {"count": 4, "covered": 2}
                result = self.check(report)
                self.assertNotEqual(result.returncode, 0)
                self.assertIn("branches 2/4 (1 unreachable)", result.stderr)

    def test_rejects_an_unexecuted_switch(self):
        result = self.check(self.report([0, 0]))
        self.assertNotEqual(result.returncode, 0)
        self.assertIn("branches 0/2 (0 unreachable)", result.stderr)

    def test_rejects_the_same_branch_in_a_different_header(self):
        report = self.report([0, 162])
        files = report["data"][0]["files"]
        files[1]["branches"] = files[0]["branches"]
        files[1]["summary"]["branches"] = {"count": 2, "covered": 1}
        result = self.check(report)
        self.assertNotEqual(result.returncode, 0)
        self.assertIn("GroupHeadHandler.h: branches 1/2 (0 unreachable)", result.stderr)

    def test_rejects_uncovered_lines_functions_and_regions(self):
        for metric in ("lines", "functions", "regions"):
            with self.subTest(metric=metric):
                report = self.report([0, 162])
                report["data"][0]["files"][0]["summary"][metric] = {"count": 2, "covered": 1}
                result = self.check(report)
                self.assertNotEqual(result.returncode, 0)
                self.assertIn(f"BoilerStateHandler.h: {metric} 1/2", result.stderr)

    def test_rejects_a_missing_header(self):
        report = self.report([0, 162])
        report["data"][0]["files"].pop()
        result = self.check(report)
        self.assertNotEqual(result.returncode, 0)
        self.assertIn("Missing coverage: StateActor.h", result.stderr)


if __name__ == "__main__":
    unittest.main()
