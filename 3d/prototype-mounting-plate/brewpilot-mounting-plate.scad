// Set "resolution" of cylinders so it isn't low poly
$fn=32;

holes = [
    [ 6.5,  6.6 ],  // RelayHole1
    [ 58,   6.5 ],  // RelayHole2
    [ 6.5, 108.1],  // RelayHole3
    [ 58,  108.1],  // RelayHole4

    [76,  6.5 ],  // ESPHole1
    [136.5, 6.5 ],  // ESPHole2
    [76,  73 ],  // ESPHole3
    [136.5, 73 ],  // ESPHole4

    [89.1, 110],  // MeanwellHole1
    [129.7, 110],  // MeanwellHole2

    [19.5,  17.1 ],  // OriginalMountHole1
    [123.5, 15.1 ],  // OriginalMountHole2
    [19.6, 125.1],   // OriginalMountHole4
    [118.5, 125.1]    // OriginalMountHole5
];

hole_diameter = 3.2; // M3

base_thickness = 2;

base_length = 140.2;
base_width  = 143;


difference() {
    cube([base_length, base_width, base_thickness]);

    for (i = [0 : len(holes)-1]) {
		h = holes[i];

        translate([h[0],
                   h[1],
                   0])
        {
            cylinder(d=hole_diameter, h=base_thickness + 0.2, $fa=100);
        }
    }
}
