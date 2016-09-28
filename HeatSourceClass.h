#pragma once
#if !defined(HEATSOURCE_H)
   #define   HEATSOURCE_H 

namespace CouponHeatXferNS
{  
   class HeatSourceClass
   {  // This is a virtual class; it cannot be instantiated.

      protected:
      public:
      std::string heatSourceName;
      double      ambientTemperatureK;

      //////////////////////////////////////////////////////////////////////////
      // Constructors //////////////////////////////////////////////////////////
      public:
      HeatSourceClass() = default;
   
      HeatSourceClass(const std::string &inFileName, const std::string &outFileName )
      {  readEchoHeatSourceParameters(inFileName, outFileName); }
   
      HeatSourceClass(std::ifstream &ifs, std::ofstream &ofs )
      {  readEchoHeatSourceParameters(ifs, ofs); }

      HeatSourceClass(const std::string &name, EnvironmentClass &env )
      : heatSourceName(name), ambientTemperatureK(env.getambientTemperatureK()) {}
     
      ~HeatSourceClass() {}

      //////////////////////////////////////////////////////////////////////////
      // Members that do useful stuff ///////////////////////////////////////////
      virtual double heatFlux(double time, double nodeTemperature) = 0;

      ////////////////// I/O Members //////////////////
      protected:
      virtual int readHeatSourceParameters    (std::ifstream &ifs ) = 0;
      virtual int echoHeatSourceParameters    (std::ofstream &ofs ) = 0;
              int readEchoHeatSourceParameters(const std::string   &inFileName, const std::string   &outFileName );
              int readEchoHeatSourceParameters(      std::ifstream &ifs       ,       std::ofstream &ofs         );
      public:

      //////////////////////////////////////////////////////////////////////////
      // Getters and Setters ///////////////////////////////////////////////////
      std::string getheatSourceName     (void              ) { return heatSourceName      ; };
      double      getambientTemperatureK(void              ) { return ambientTemperatureK ; };

      void        setheatSourceName     (std::string value ) { heatSourceName      = value; };
      void        setambientTemperatureK(double      value ) { ambientTemperatureK = value; };
   } ; // End of class HeatSource

   class ConvectionClass : public HeatSourceClass
   {  
      // I deleted the Natural and Forced convection classes; the Convection
      // class is suitable for both purposes.
      //
      // Convection coefficients are a function of free stream velocity, wall
      // temperature, freestream temperature, and position on the surface.
      // The latter is beyond the scope of what I'm trying to do here.
      // However, if the temperatures (or speed) vary substantially across
      // the surfaces or over time, new convection coefficients may have to be
      // calculated. That won't be difficult code to write, and don't think it
      // will be a performance issue unless I do something crazy like
      // instantiate one (or two) convection objects for each node.  I'm
      // planning on a simpler approach: one object for forced convection, 
      // a second for natural convection. I'll probably update the convection
      // coefficients for each node at each timestep.
      // The wall temperature is not a proper field of the convection class,
      // but I might want know when the wall temperature has changed
      // sufficiently to warrent recalculating the convection coefficient.
      // I'll save the last wall temperature used for that purpose.
      protected:
//    std::string heatSourceName;
      double      freestreamSpeed     ;
      std::string convectionType      ;
      double      hConvection         ; // May or may not be recalcutated for each
                                        // node at each time step.
      double      lastWallTemperaturK ; // Most recent wall temperature used to
                                        //  calculatethe convection coefficient.

      public:
      ConvectionClass(const std::string &name, EnvironmentClass &env, double speed)
              :   HeatSourceClass(name, env)
               , freestreamSpeed(fabs(speed))
               , convectionType(speed==0? "Natural" : "Forced") {}

      ConvectionClass(const std::string &inFileName, const std::string &outFileName  = "")
            
      {  readEchoHeatSourceParameters(inFileName, outFileName); }

      ConvectionClass(std::ifstream &ifs, std::ofstream &ofs )
      {  readEchoHeatSourceParameters(ifs, ofs); }

      public:
      int readHeatSourceParameters(std::ifstream &ifs ) override; // Place holder for now.
      int echoHeatSourceParameters(std::ofstream &ofs ) override; // Place holder for now.

      protected:
      double calculateHConv(double airspeed, double tAmb, double tWall);

      public:
      virtual double heatFlux(double time, double nodeTemperature) override;
      double updateHConv     (double airspeed, double tAmb, double tWall);

      double      getfreestreamSpeed    (void              ) { return freestreamSpeed     ; };
      double      getlastWallTemperaturK(void              ) { return lastWallTemperaturK ; };
      std::string getconvectionType     (void              ) { return convectionType      ; };
      double      gethConvection        (void              ) { return hConvection         ; };

      void        setfreestreamSpeed    (double      value ) { freestreamSpeed     = value; };
      void        setlastWallTemperaturK(double      value ) { lastWallTemperaturK = value; };
      void        setconvectionType     (std::string value ) { convectionType      = value; };
      void        sethConvection        (double      value ) { hConvection         = value; };

   }; // End of class ConvectionClass

   class RadiationClass : public HeatSourceClass
   {  private:
      std::string heatSourceName ;
      double      emissivity;

      public:
      RadiationClass(const std::string &name, EnvironmentClass &env, double emiss)
              :   HeatSourceClass(name, env),  emissivity(emiss)
      {   heatSourceName = name;
          ambientTemperatureK = env.getambientTemperatureK();
      }


      RadiationClass(const std::string &inFileName, const std::string &outFileName  = "")
      {  readEchoHeatSourceParameters(inFileName, outFileName); }

      RadiationClass(std::ifstream &ifs, std::ofstream &ofs )
      {  readEchoHeatSourceParameters(ifs, ofs); }

      protected:
      int readHeatSourceParameters(std::ifstream &ifs ) override; // What is there to read? emissivity? Band limits?
      int echoHeatSourceParameters(std::ofstream &ofs ) override;

      public:
      double heatFlux     (double time, double nodeTemperature) override;
      double bandRandiance(double nodeTemperature, double wavelengthLow, double wavelengthHi);

      // I should get the emissivity from the coupon object.
      double getemissivity         (void        ) { return emissivity           ; }
                                    
      void   setemissivity         (double value) { emissivity          = value; }

   }; // End of class ForcedConvection


   class CombustionClass : public HeatSourceClass
   {  // Thinking about making this a singleton class, or possibly a static class.
      // I'm not sure about the combustion model. It should be a
      // function of surface temperature and remaining fuel. Potentially
      // oxygen availability (air flow across the surface, altitude, ...)
      // could be a factor. I'll start with a simple model and see how that
      // compares with the test data.
      // Proposed models:
      // 1) Heat impulse at (t=0) = hPulse.
      //    Heat Flux    at (t>0) = (hfuel(t)/hfuel(0)^fuelExp)*refRate*(2^((tWall-tRef)/10))
      //    where
      //      hfuel(0) [KJ/m^2    ]  Intial fuel available (Del H per m^2)
      //      hfuel(t) [KJ/m^2    ]  Remaining Fuel or energy (Del H)             
      //      fuelExp  [ unitless ]  Exonent to tweak the model.
      //                             E.g. account for  products of combustion
      //                             becoming a barrier between remaining fuel
      //                             and Oxygen.
      //      refRate  [KJ/m^2 sec]  Reaction rate at the reference temperature
      //                             with Fuel(0) available fuel.
      //      tRef     [  K       ]  Reference temperature for the reaction rate. 
      //      tWall    [  K       ]  Node temperature.                            
      //      hPulse   [ KJ       ]  Fudge factor to get things heated up.
      //                            Likely value for Kfudge is the energy
      //                             required to heat the coupon to around 200 C.
      protected:
      // "Input" values passed to constructor or read from input file.
      int    reactionModel ;  // Flag indicating chemical reaction model. Should I create another enum class?
      double hPulse        ;  // [KJ/m^2    ] 
      double hfuel0        ;  // [KJ/m^2    ] = hReaction
      double fuelExp       ;  // [unitless  ]
      double refRate       ;  // [KJ/m^2/sec]
      double tRef          ;  // [K         ]
//    double temperatureExp;  // [K         ]

      public:
      CombustionClass(const std::string &name, EnvironmentClass &env, int    model, double pulse, 
                     double fuelInit, double fExp, double rRate, double rTemp, double tempExp)
           :   HeatSourceClass(name, env)
             , reactionModel  (model    )
             , hPulse         (pulse    )
             , hfuel0         (fuelInit )
             , fuelExp        (fExp     )
             , refRate        (rRate    )
             , tRef           (rTemp    ) {}
//          , temperatureExp  (tempExp  ) {}
      CombustionClass(const std::string &inFileName, const std::string &outFileName  = "")
      {  readEchoHeatSourceParameters(inFileName, outFileName); }

      CombustionClass(std::ifstream &ifs, std::ofstream &ofs )
      {  readEchoHeatSourceParameters(ifs, ofs); }

      public:
      double heatFlux(double time, double nodeTemperature) override;
      double decrementFuel(double delFuel);

      protected:
      int readHeatSourceParameters(std::ifstream &ifs ) override; // What is there to read? emissivity? Band limits?
      int echoHeatSourceParameters(std::ofstream &ofs ) override;

      int    getreactionModel  (void        ) { return reactionModel  ; }
      double gethPulse         (void        ) { return hPulse         ; }
      double gethfuel0         (void        ) { return hfuel0         ; }
      double getfuelExp        (void        ) { return fuelExp        ; }
      double getrefRate        (void        ) { return refRate        ; }
      double gettRef           (void        ) { return tRef           ; }
//    double getemperatureExp  (void        ) { return temperatureExp ; }
//    double getfuelRemaining  (void        ) { return fuelRemaining  ; }

      void   setreactionModel  (int    value) { reactionModel  = value; }
      void   sethPulse         (double value) { hPulse         = value; }
      void   sethfuel0         (double value) { hfuel0         = value; }
      void   setfuelExp        (double value) { fuelExp        = value; }
      void   setrefRate        (double value) { refRate        = value; }
      void   settRef           (double value) { tRef           = value; }
//    void   settemperatureExp (double value) { temperatureExp = value; }
//    void   setfuelRemaining  (double value) { fuelRemaining  = value; }

   }; // End of class CombustionClass

#define INHERIT_CONSTRUCTOR(Derived,Base)                             \
      template <class... Args> Derived(Args&&... args)                \
                              : Base(std::forward<Args>(args)...) { }

   class CombustionClass0 : public CombustionClass
   {
      // Use CombustionClass::CombustionClass construcor
      // INHERIT_CONSTRUCTOR(CombustionClass0, CombustionClass)
      CombustionClass0(std::ifstream &ifs, std::ofstream &ofs) : CombustionClass(ifs, ofs) {}

   }; // End of class CombustionClass
   class CombustionClass1 : public CombustionClass
   {
      // Use CombustionClass::CombustionClass construcor
      // INHERIT_CONSTRUCTOR(CombustionClass0, CombustionClass)
      CombustionClass1(std::ifstream &ifs, std::ofstream &ofs) : CombustionClass(ifs, ofs) {}

   }; // End of class CombustionClass
   class CombustionClass2 : public CombustionClass
   {
      // Use CombustionClass::CombustionClass construcor
      // INHERIT_CONSTRUCTOR(CombustionClass0, CombustionClass)
      CombustionClass2(std::ifstream &ifs, std::ofstream &ofs) : CombustionClass(ifs, ofs) {}

   }; // End of class CombustionClass
   class CombustionClass3 : public CombustionClass
   {
      // Use CombustionClass::CombustionClass construcor
      // INHERIT_CONSTRUCTOR(CombustionClass0, CombustionClass)
      CombustionClass3(std::ifstream &ifs, std::ofstream &ofs) : CombustionClass(ifs, ofs) {}

   }; // End of class CombustionClass
   class CombustionClass4 : public CombustionClass
   {
      // Use CombustionClass::CombustionClass construcor
      // INHERIT_CONSTRUCTOR(CombustionClass0, CombustionClass)
      CombustionClass4(std::ifstream &ifs, std::ofstream &ofs) : CombustionClass(ifs, ofs) {}

   }; // End of class CombustionClass

} // End of Namespace CouponHeatXferNS

#endif // HEATSOURCE_H 
