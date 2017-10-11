#version 3.7;

global_settings {
  assumed_gamma 1.0
}

camera {
  right 2*x
  location <0, 0, -10>
  look_at <0, 0, 0>
  angle 24
}

light_source { <-5, 20, -10>, color rgb 1 }
light_source { <5, 20, -10>, color rgb 1 }

// Vuvuzela, with mouthpiece around <2, 0, 0>, flare around <-2, 0, 0>, and max
// diameter around 1.
#declare Vuvuzela = lathe {
  bezier_spline
  24,
  // mouthpiece
  <0.001, 0.2>, <0.001, 0.2>, <0.06, 0.11>, <0.06, 0.1>,
  <0.06, 0.1>, <0.06, 0.09>, <0.07, 0.09>, <0.07, 0.1>,
  <0.07, 0.1>, <0.07, 0.15>, <0.05, 0.19>, <0.04, 0.2>,
  // rest of the vuvuzela
  <0.04, 0.2>, <0.04125, 0.6>, <0.1, 3.5>, <0.5, 3.9>,
  <0.5, 3.9>, <0.51, 3.91>, <0.51, 3.92>, <0.5, 3.92>,
  <0.5, 3.92>, <0.49, 3.92>, <0.1, 3.8>, <0.001, 3.8>
  sturm

  translate -2*y
  scale 0.9
  rotate 90*z
};

#declare VuvuzelaColors = array[3] {
  color rgb <1, 0, 0>,
  color rgb <0, 1, 0>,
  color rgb <0, 0, 1>,
};
#declare VuvuzelaColorCount = dimension_size(VuvuzelaColors, 1);

#declare VuvuzelaNumber = 0;
#for (Exponent, 0, 10)
  #declare ColumnCount = pow(2, Exponent);
  #for (Column, 0, ColumnCount - 1)
    object {
      Vuvuzela

      no_shadow

      texture {
        pigment {
          color VuvuzelaColors[mod(VuvuzelaNumber, VuvuzelaColorCount)]
        }
        finish {
          ambient 0.1
          phong 0.5 phong_size 40
        }
        normal {
          bozo 0.2
          scale 0.002
        }
      }

      rotate 180 * mod(Exponent, 2) * y
      translate <2, -0.5, 0>
      scale 1 / ColumnCount
      translate <-2 + 4 * Column / ColumnCount, -1 + 2 / ColumnCount, 0>
    }
    #declare VuvuzelaNumber = VuvuzelaNumber + 1;
  #end
#end
