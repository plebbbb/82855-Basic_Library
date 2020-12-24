//A header guard
#pragma once


#ifndef HOLONOMIC_HPP
#define HOLONOMIC_HPP

  namespace STL_lib{
    struct holonomicbase;
    struct motorwheel;

    struct motorwheel : public pros::Motor{
      inches diameter;
      SMART_radians heading;
      double SINE, COSINE;
      /*WARNING: if you want to use degrees, cast your value to degrees
      e.g: (degrees)45 would be a proper degrees value, while 45 would be 45 radians

      ALL YOUR MOTORS SHOULD HAVE THEIR POSITIVE DIRECTION ROTATE THE BOT CCW
      IN DETERMINE THEIR HEADINGS BASED ON THE FACT THAT 0 RAD is DIRECTLY RIGHT*/
      motorwheel(pros::Motor mot, inches d, SMART_radians direction):Motor(mot){
        diameter = d; heading = direction; SINE = sin(heading); COSINE = cos(heading);
      }
    };

    struct holonomicbase{
      std::vector<motorwheel> motors;

      /******************************************************************************/
      //Constructors:
      holonomicbase operator= (std::vector<motorwheel> set){return holonomicbase(set);}
      holonomicbase(std::vector<motorwheel> set):motors(set){}

      /******************************************************************************/
      //Utility functions
      coordinate tare(coordinate unscaled){
        inches speedcap = (unscaled.x > unscaled.y) ? unscaled.x : unscaled.y;
        if (speedcap == 0) return std::pair<inches,inches>{0,0};
        return std::pair<inches,inches>{unscaled.x/speedcap, unscaled.y/speedcap};
      } //questionable function name

      /******************************************************************************/
      //Primary computation functions
      /*Heading is coordinates used to describe a local vector relatice to the bot, whereby right is 0rad
      rotationfactor is a scaling value with an interval from -1 to 1 to determine distribution of power
      to rotation with sign for direction*/
      void move_vector_RF(coordinate heading, double rotationfactor, percent speed){
        heading = tare(heading);
        for(motorwheel temp : motors){ //holy shit this exists in c++
          temp.move_velocity(2*speed*((heading.x*temp.COSINE + heading.y*temp.SINE)*(1-rotationfactor) + rotationfactor));
          //multiply by 2 at very end due to move_velocity having a default interval of -200 to 200
        }
      }

      //automatic rotation weighting and conversion
      void move_vector_RAW(coordinate heading, double rotationRAW, percent speed){
        heading = tare(heading);
        rotationRAW/=(sqrt(heading.x*heading.x+heading.y*heading.y) + fabs(rotationRAW));
        for(motorwheel temp : motors){ //holy shit this exists in c++
          temp.move_velocity(2*speed*((heading.x*temp.COSINE + heading.y*temp.SINE)*(1-rotationRAW) + rotationRAW));
          //multiply by 2 at very end due to move_velocity having a default interval of -200 to 200
        }
      }

      //direct local axis control of bot. Rotate 45 degrees CCW from standard heading
      //power output interval of 0-100 for both rotation and translation
      void move_perp_vector_xdrive(percent FB, percent LR, percent R){
        for(motorwheel temp : motors){
          temp.move_voltage((double(FB * cos(temp.heading+M_PI/4)+ LR*sin(temp.heading+M_PI/4)) + double(R))*(12000/200));
          //Move_velocity actually has its own internal PID loop. Using it at close range makes everything super noisy.
          //Therefore, we are going to be directly taking percent inputs, which should have their own PIDs.
        }
      };


    };
  }

#endif
