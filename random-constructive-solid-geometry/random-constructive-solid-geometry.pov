#version 3.7;

#include "colors.inc"
#include "rand.inc"
#include "shapes3.inc"
#include "textures.inc"
#include "transforms.inc"

// For creating miniscule gaps that avoid floating point issues.
#declare Slop = 5e-5;

global_settings {
  assumed_gamma 1.0
}

camera {
  right x*16.0/9.0
  location <0, 5, -10>
  look_at <0, 5, 0>
  angle 75
}

// Base floor.
plane {
  y, 0
  texture {
    pigment {
      checker color rgb 1, color rgb 0
    }
  }
}

// Walls and ceiling.
box {
  <-15, -1, -15>, <15, 10, 15>
  hollow
  texture {
    pigment {
      color rgb 1
    }
    finish {
      specular 0.5
      roughness 0.05
    }
  }
}

// Lights.
light_source { <-3, 8, 0>, color rgb 0.3 }
light_source { < 0, 8, 0>, color rgb 0.3 }
light_source { < 3, 8, 0>, color rgb 0.3 }

// Shapes of the columns below the interesting shapes.
#macro SupportColumns(Radius_Delta, Bottom_Delta, Top_Delta)
  #local height_delta = Top_Delta - Bottom_Delta;

  union {
    #for (column_idx, 0, 4)
      #local num_sides = 3 + 2 * column_idx;
      #local height = 1 + column_idx + height_delta;
      #local circumcircle_radius = 1 + Radius_Delta;
      #local position = <-7 + 3.5 * column_idx, 0, 5>;

      #local unit_incircle_diameter = 1 / tan(pi / num_sides);
      #local unit_circumcircle_diameter = 1 / sin(pi / num_sides);
      #local incircle_radius = circumcircle_radius * unit_incircle_diameter / unit_circumcircle_diameter;

      object {
        Column_N(num_sides, incircle_radius, height)
        translate position + Bottom_Delta * y
      }
    #end
  }
#end

// Inner columns.
object {
  SupportColumns(-Slop, Slop, -Slop)
  texture {
    pigment {
      color rgb 0
    }
  }
}

// Glass on floor and around columns.
difference {
  merge {
    box { <-15 + Slop, Slop, -15 + Slop>, <15 - Slop, 0.05, 15 - Slop> }
    SupportColumns(0.05, Slop, 0.05)
  }
  SupportColumns(Slop, -Slop, Slop)

  texture {
    pigment {
      color rgbf <1, 1, 1, 0.8>
    }
    finish {
      specular 1
      roughness 0.001
      ambient 0
      diffuse 0
      reflection 0.05
    }
  }
  interior {
    Glass_Interior
  }
}

// Create a single shape for adding to the resulting object of RandomCSG.
#macro ShapeForAdding()
  object {
    #switch (RRand(0, 1, RdmA))
      #range (0.0, 0.5)
        sphere { <0, 0, 0>, 1 }
      #break
      #range (0.5, 0.7)
        cylinder { <0, -1, 0>, <0, 1, 0>, 0.5}
      #break
      #range (0.7, 0.9)
        torus { 0.75, 0.25 }
      #break
      #range (0.9, 1.0)
        box { <-1, -1, -1>, <1, 1, 1> }
      #break
    #end
    scale <pow(2, RRand(-2, 1, RdmA)), pow(2, RRand(-2, 1, RdmA)), pow(2, RRand(-2, 1, RdmA))>
    Point_At_Trans(VRand_On_Sphere(RdmA))
    translate VRand_In_Box(<-1, -1, -1>, <1, 1, 1>, RdmA)
  }
#end

// Create a single shape for subtracting from the resulting object of
// RandomCSG.
#macro ShapeForSubtracting()
  object {
    #switch (RRand(0, 1, RdmA))
      #range (0.0, 0.25)
        plane { y, RRand(-1.0, -0.5, RdmA) }
      #break
      #range (0.25, 1.0)
        sphere { <0, RRand(-1.5, -1.0, RdmA), 0>, RRand(0.25, 0.75, RdmA) }
      #break
    #end
    Point_At_Trans(VRand_On_Sphere(RdmA))
  }
#end

// Create a material for RandomCSG.
#macro RandomMaterial()
  material {
    texture {
      pigment {
        #switch (RRand(0, 1. RdmA))
          #range (0.0, 0.1)
            color rgbf <1, 1, 1, RRand(0.7, 0.9, RdmA)>
          #break
          #range (0.1, 1.0)
            color CHSL2RGB(<RRand(0, 360, RdmA), 1, 0.5, RRand(0.6, 0.8, RdmA)>)
          #break
        #end
      }
      finish {
        Glass_Finish
      }
    }
    interior {
      Glass_Interior
    }
  }
#end

// Create a single object from random CSG, *roughly* within the box from
// <-1, -1, -1> to <1, 1, 1>.
#macro RandomCSG(AddCount, SubtractCount)
  object {
    difference {
      merge {
        #for (sub_object_number, 1, AddCount)
          ShapeForAdding()
        #end
      }
      #for (sub_object_number, 1, SubtractCount)
        ShapeForSubtracting()
      #end
    }
    RandomMaterial()
  }
#end

// The random CSG objects themselves.
#for (column_idx, 0, 4)
  object {
    RandomCSG(3 + 2 * column_idx, column_idx)
    scale 0.75
    translate <-7 + 3.5 * column_idx, 3 + column_idx, 5>
  }
#end
