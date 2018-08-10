#version 3.7;

#include "colors.inc"
#include "rand.inc"
#include "textures.inc"
#include "transforms.inc"

global_settings {
  assumed_gamma 1.0
}

camera {
  right x*16.0/9.0
  location <0, 0, -10>
  look_at <0, 0, 0>
  angle 50
}

light_source { <5, 20, -10>, color rgb 1 }

// Background
plane {
  -z, -10
  texture {
    pigment {
      color rgb 1
    }
    finish {
      ambient 1
    }
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
#macro RandomCSG()
  object {
    difference {
      merge {
        #for (sub_object_number, 1, 8)
          ShapeForAdding()
        #end
      }
      #for (sub_object_number, 1, 3)
        ShapeForSubtracting()
      #end
    }
    RandomMaterial()
  }
#end

#for (object_number, 1, 60)
  object {
    RandomCSG()
    scale 0.3
    translate VRand_In_Box(<-6, -3, -1>, <6, 3, 1>, RdmA)
  }
#end
