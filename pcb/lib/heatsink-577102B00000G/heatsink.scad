length = 19.05;
width = 13.21;
fin_height = 9.52;
thickness = 1.27;

cube([width, thickness, length]);
translate([0, thickness, 0]) {
    cube([thickness, fin_height, length]);
}
translate([width - thickness, thickness, 0]) {
    cube([thickness, fin_height, length]);
}