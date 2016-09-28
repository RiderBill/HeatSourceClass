#include "stdafx.h"
#include <wcbNamespace.h>
#include "CouponHeatXferAllIncludes.h"
//#include "NonClassHeader.h"
//#include "HeatSourceClass.h"

namespace CouponHeatXferNS
{
   int HeatSourceClass::readEchoHeatSourceParameters(const std::string &inFileName, const std::string &outFileName )
   {
      std::ifstream ifs(inFileName, std::ios_base::in);
      if(!(ifs.good()&&ifs.is_open())) return (int)RETURNCODE::INVALID_IFS; 
      readHeatSourceParameters (ifs); ifs.seekg(0, std::ios::beg); 

      if(outFileName.length() > 0)
      { std::ofstream ofs(outFileName, std::ios_base::out);
         if(!(ofs.good()&&ofs.is_open())) return (int)RETURNCODE::INVALID_OFS; 
         echoHeatSourceParameters (ofs);
      }
      return 0;
   } // End of HeatSourceClass::readWriteHeatSourceParameters(inFileName, outFileName)

   int HeatSourceClass::readEchoHeatSourceParameters(std::ifstream &ifs, std::ofstream &ofs)
   {
      if(!(ifs.good()&&ifs.is_open())) return (int)RETURNCODE::INVALID_IFS; 
      readHeatSourceParameters (ifs); ifs.seekg(0, std::ios::beg); 

      if((ofs) && ofs.is_open())
         echoHeatSourceParameters (ofs);
      return 0;
   } // End of HeatSourceClass::readWriteHeatSourceParameters(const std::ifstream &ifs, const std::ofstream &ofs)

///////////////////////////////////////////////////////////////////////////////
/////////////////////////// CombustionClass Members ///////////////////////////

// CombustionClass::CombustionClass(const std::string &inFileName, const std::string &outFileName )
// {  readWriteHeatSourceParameters(inFileName, outFileName);
// }
//
// ConvectionClass::ConvectionClass(const std::string &inFileName, const std::string &outFileName )
// {  readWriteHeatSourceParameters(inFileName, outFileName);
// }
//
// RadiationClass::RadiationClass(const std::string &inFileName, const std::string &outFileName )
// {  readWriteHeatSourceParameters(inFileName, outFileName);
// }

//   int CombustionClass::readHeatSourceParameters(std::ifstream &ifs ){}

   int CombustionClass::readHeatSourceParameters(std::ifstream &ifs )
   {  WCB_CONVENIENT_DEBUG_STRINGS;
      if(!(ifs.good()&&ifs.is_open())) return (int)RETURNCODE::INVALID_IFS; 
      std::string str(""); // Declare outside the try block to be in scope of
                           // the catch blocks.
      try
      {  // Search for start of coupon properties.
LNO      wcb::findString(ifs, (str="Chemical Energy Parameters"));
LNO      wcb::findString(ifs, (str="reactionModel")); ifs >> reactionModel  ;
LNO      wcb::findString(ifs, (str="hPulse       ")); ifs >> hPulse         ;
LNO      wcb::findString(ifs, (str="hfuel0       ")); ifs >> hfuel0         ;
LNO      wcb::findString(ifs, (str="fuelExp      ")); ifs >> fuelExp        ;
LNO      wcb::findString(ifs, (str="refRate      ")); ifs >> refRate        ;
LNO      wcb::findString(ifs, (str="tRef         ")); ifs >> tRef           ;
LNO      str.clear(); // Don't want a misleading string in an exception message.
      }
      catch(const wcb::wcbFindStringNotFoundException &e)
      {  std::string eInfo(WCB_FCNNAME_S + WCB_FCNSIG_S + WCB_FCNNAMEUA_S+ WCB_LINE_NUMBER_S
                           + "\"String \"" + str + "\" not found." );
         throw wcb::wcbFindStringNotFoundException(e,  eInfo);
      }
      catch(const std::logic_error &e)
      {  std::string eInfo(WCB_FCNNAME_S + WCB_FCNSIG_S + WCB_FCNNAMEUA_S+ WCB_LINE_NUMBER_S);
         throw wcb::wcbExceptionB(e, "Unknown exception\n" + eInfo);
      }

      return 0;
   } // End of CombustionClass::read_HeatSourceParameters

    int CombustionClass::echoHeatSourceParameters(std::ofstream &ofs )
   {  WCB_CONVENIENT_DEBUG_STRINGS;
      if(!(ofs.good()&&ofs.is_open())) return (int)RETURNCODE::INVALID_OFS; 
      ofs.fill(' '); ofs << std::right; ofs << std::setprecision(2) << std::fixed ;
      ofs << "____________________________________________________________________________________\n";
      ofs << "\n";
      ofs << "Chemical Energy Parameters\n";
      ofs <<                                                                                      "\n";
      ofs << "Reaction Models:                                                                     \n";
      ofs << "1) Heat impulse at (t=0) = hPulse.                                                   \n";
      ofs << "   Heat Flux    at (t>0) = (hfuel(t)/hfuel(0)^fuelExp)*refRate*(2^((tWall-tRef)/10)) \n";
      ofs << "   where                                                                             \n";
      ofs << "     hfuel(0) [KJ/m^2    ]  Intial fuel available (Del H per m^2)                    \n";
      ofs << "     hfuel(t) [KJ/m^2    ]  Remaining Fuel or energy (Del H)                         \n";
      ofs << "     fuelExp  [ unitless ]  Exonent to tweak the model.                              \n";
      ofs << "                            E.g. account for  products of combustion                 \n";
      ofs << "                            becoming a barrier between remaining fuel                \n";
      ofs << "                            and Oxygen.                                              \n";
      ofs << "     refRate  [KJ/m^2 sec]  Reaction rate at the reference temperature               \n";
      ofs << "                            with Fuel(0) available fuel.                             \n";
      ofs << "     tRef     [  K       ]  Reference temperature for the reaction rate.             \n";
      ofs << "     tWall    [  K       ]  Node temperature.                                        \n";
      ofs << "     hPulse   [ KJ       ]  Fudge factor to get things heated up.                    \n";
      ofs << "                            Likely value for hPulse is the energy                    \n";
      ofs << "                            required to heat the coupon to around 100 C.             \n";
      ofs <<                                                                                      "\n";
      ofs << "Reaction model parameters depend on which model is specified.                        \n";
      ofs <<                                                                                      "\n";
      ofs << "reactionModel    "; ofs.width( 3); ofs << getreactionModel () << ""              << "\n";
      ofs << "hPulse           "; ofs.width( 6); ofs << gethPulse        () << " [KJ/m^2    ]" << "\n";
      ofs << "hfuel0           "; ofs.width( 6); ofs << gethfuel0        () << " [KJ/m^2    ]" << "\n";
      ofs << "fuelExp          "; ofs.width( 6); ofs << getfuelExp       () << " [unitless  ]" << "\n";
      ofs << "refRate          "; ofs.width( 6); ofs << getrefRate       () << " [KJ/m^2/sec]" << "\n";
      ofs << "tRef             "; ofs.width( 6); ofs << gettRef          () << " [K         ]" << "\n";
//    ofs << "temperatureExp   "; ofs.width( 6); ofs << gettemperatureExp() << "[unitless  ]" << "\n";
      ofs << std::endl;
      return 0;
   } // End of CombustionClass::writeHeatSourceParameters

   double CombustionClass::heatFlux(double time, double nodeTemperature)
   {  return 0;
   } // End of CombustionClass::heatFlux


///////////////////////////////////////////////////////////////////////////////
/////////////////////////// ConvectionClass Members ///////////////////////////

   int ConvectionClass::readHeatSourceParameters(std::ifstream &ifs )
   {  // Read a table of fluid properties, etc, for use in calculating
      // convection coefficients? Sounds good. I need those properties.
      return 0;
   } // End of Convectoion::read_HeatSourceParameters

   int ConvectionClass::echoHeatSourceParameters(std::ofstream &ofs )
   {  // Echo back the table of fluid properties.
      return 0;
   } // End of ConvectionClass::writeHeatSourceParameters

   double ConvectionClass::heatFlux(double time, double nodeTemperature)
   { return 0.0;
   } // End of ConvectionClass::heatFlux()


///////////////////////////////////////////////////////////////////////////////
/////////////////////////// RadiationClass Members ////////////////////////////

  int RadiationClass::readHeatSourceParameters(std::ifstream &ifs )
   {  // Echo back the table of fluid properties.
      return 0;
   } // End of Convectoion::read_HeatSourceParameters

   int RadiationClass::echoHeatSourceParameters(std::ofstream &ofs )
   {  // Echo back the table of fluid properties.
      return 0;
   } // End of Convectoion::writeHeatSourceParameters

   //////////////////////////////////////////////////////////////////////////
   // Members that do useful stuff ///////////////////////////////////////////
   double RadiationClass::heatFlux(double time, double nodeTemperature)
   { return 0.0;
   } // End of RadiationClass::heatFlux()

   

} // End of Namespace CouponHeatXferNS
