// This laser module holder can be used to attach a laser module to a
// solderless breadboard (not an optical breadboard).

laser_module_diameter = 7;
tolerance = 0.5;
laser_module_length = 21;
laser_part_thickness = 2.5;
base_thickness = 2;
set_screw_diameter = 1.8;
base_width = 25;
breadboard_spacing = 2.54;
breadboard_hole_diameter = 1;

// smoothness of circles
$fn = 60;


module laser_holder_part_2d() {
  difference() {
    union() {
      // outer cylinder for thickness of laser module holder
      circle(d = laser_module_diameter + tolerance + 2*laser_part_thickness);
      
      // part that connects the laser module hole to the base
      translate([-(laser_module_diameter + tolerance + 2*laser_part_thickness)/2, -10, 0]) {
        square([laser_module_diameter + tolerance + 2*laser_part_thickness, 10]);
      }
    }

    // hole for laser module
    circle(d = laser_module_diameter + tolerance);
  }
}


module base_2d() {

  translate([-(base_width/2), -10, 0]) {
    square([base_width, base_thickness]);  
  }
}

// Fillet module taken from the OpenSCAD user manual: 
// https://en.wikibooks.org/wiki/OpenSCAD_User_Manual/Transformations#offset
module fillet(radius) {
   offset(r = -radius) {
     offset(delta = radius) {
       children();
     }
   }
}

module laser_module_holder() {
  difference() {
    difference() {
      linear_extrude(laser_module_length) {
        fillet(2) {
          laser_holder_part_2d();
          base_2d();
        }
      }

      // set screw hole
      translate([0, (laser_module_diameter + tolerance)/2 + laser_part_thickness, laser_module_length/2]) {
        rotate([90,0,0]){
          cylinder(h=laser_part_thickness + 0.1, d=set_screw_diameter, center=false);
        }
      }

    }

    // breadboard mounting holes
    translate([breadboard_spacing*4, -10 + base_thickness/2, laser_module_length/4]) {
      cube([breadboard_hole_diameter, base_thickness, breadboard_hole_diameter], center=true);
    }

    translate([-breadboard_spacing*4, -10 + base_thickness/2, laser_module_length/4]) {
      cube([breadboard_hole_diameter, base_thickness, breadboard_hole_diameter], center=true);
    }

    translate([breadboard_spacing*4, -10 + base_thickness/2, laser_module_length/4 + breadboard_spacing*4]) {
      cube([breadboard_hole_diameter, base_thickness, breadboard_hole_diameter], center=true);
    }

    translate([-breadboard_spacing*4, -10 + base_thickness/2, laser_module_length/4 + breadboard_spacing*4]) {
      cube([breadboard_hole_diameter, base_thickness, breadboard_hole_diameter], center=true);
    }
  }
}

laser_module_holder();