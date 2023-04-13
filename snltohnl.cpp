#include <iostream>
#include <boost/filesystem/fstream.hpp>

using namespace std;
using namespace NAJA::SNL;


#include "SNLUniverse.h";
#include "SNLDB.h";
#include "SNLLibrary.h";
#include "SNLDESIGN.h";
#include "SNLScalarTerm.h"; 
#include "SNLBusTerm.h";
#include "SNLBusTerm.h";
#include "SNLBusTermBit.h";
#include "SNLInstance.h";
#include "SNLScalarNet.h";
#include "SNLBusNet.h";
#include "SNLBusNetBit.h";

#include "SNLBusTermIterators.h";

// [Creating SNLDB]
SNLUniverse* universe = SNLUniverse::get ();
if (!universe) {
     SNLUniverse::create ()
};
SNLDB* db= SNLDB::create(universe);
SNLLibrary* primitivesLib=SNLLibrary::create (SNLDB db, SNLName("PRIMITIVES_LIBRARY"));
SNLLibrary* modelLib=SNLLibrary::create (SNLDB db, SNLName("MODEL_LIBRARY"));
SNLLibrary* workLib=SNLLibrary::create (SNLDB db, SNLName("WORK_LIB"));
// [Creating SNLDB]



// [Creating SNLDesign]
SNLDesign* lut2 = SNLDesign::create(primitivesLib, SNLName("LUT2")); 
SNLDesign* model0 = SNLDesign::create(modelLib, SNLName("Model0"));
SNLDesign* model1 = SNLDesign::create(modelLib, SNLName("Model1"));
SNLDesign* top = SNLDesign::create(workLib, SNLName("Top"));
// [Creating SNLDesign]


// [Creating Interface]
//[LUT2]
SNLScalarTerm::create (lut2, Direction Input, SNLName("I0"));
SNLScalarTerm::create (lut2, Direction Input, SNLName("I1"));
SNLScalarTerm::create (lut2, Direction Output, SNLName("o"));

//[Model0]
SNLScalarTerm::create (model0, Direction Input, SNLName("in0"));
SNLScalarTerm::create (model0, Direction Input, SNLName("in1"));
SNLScalarTerm::create (model0, Direction Input, SNLName("out"));

//[Model1]
SNLScalarTerm::create (model1, Direction Input, SNLName("in0"));
SNLScalarTerm::create (model1, Direction Input, SNLName("in1"));
SNLScalarTerm::create (model1, Direction Output, SNLName("out"));

//[Top]
SNLBusTerm::create (top, Direction Input, 2, 0, SNLName("in"));
SNLScalarTerm::create (top, Direction Output, SNLName("out"));
// [Creating Interface]

//[Creating Nets]
//model1
{
     SNLScalarNet* in0Net=SNLScalarNet::create (model1, SNLName("in0"));
     SNLScalarNet* in1Net=SNLScalarNet::create (model1, SNLName("in1"));
     SNLScalarNet* outNet=SNLScalarNet::create (model1, SNLName("out"));
     model1->getScalarTerm(HNLName("in0"))->setNet(in0Net);
     model1->getScalarTerm(HNLName("in1"))->setNet(in1Net);
     model1->getScalarTerm(HNLName("out"))->setNet(outNet);

//model0
{
     SNLScalarNet* in0Net=SNLScalarNet::create (model0, SNLName("in0"));
     SNLScalarNet* in1Net=SNLScalarNet::create (model0, SNLName("in1"));
     SNLScalarNet* outNet=SNLScalarNet::create (model0, SNLName("out"));
     model0->getScalarTerm(HNLName("in0"))->setNet(in0Net);
     model0->getScalarTerm(HNLName("in1"))->setNet(in1Net);
     model0->getScalarTerm(HNLName("out"))->setNet(outNet);
}

//top
{
     SNLBusNet* inNet = SNLBusNet::create(top, 2, 0, SNLName("in"));
     SNLScalarNet* outNet = SNLScalarNet::create(top, SNLName("out"));
     //Iterate over bus term bits and connect them
     SNLBusTerm* inBusTerm = top->getBusTerm(SNLName("in"));
     for (SNLBusTermBitsIterator btit(inBusTerm); btit.isValid(); ++btit) {
          SNLBusTermBit* busTermBit = *btit;
          int bit = busTermBit->getBitIndex();
           SNLBusNetBit* busNetBit = inNet->getNetBit(bit);
           busTermBit->setNet(busNetBit);
      top->getScalarTerm(SNLName("out"))->setNet(outNet);
}
//[Creating Nets]

//[creatin instances]
SNLInstance::create(model1, lut2, SNLName("lut")); 
SNLInstance::create(model0, model1, SNLName("ins0")); 
SNLInstance::create(top, model0, SNLName("ins0"));
SNLInstance::create(top, model1, SNLName("ins1"));
//[creatin instances]

//[conncet instances]
{
     //Connect ins0 to model0
     SNLInstance* lut = model1->getInstance(SNLName("lut"));
     SNLScalarTerm* lut2_I0 = lut2->getScalarTerm(SNLName("I0"));
     SNLScalarTerm* lut2_I1 = lut2->getScalarTerm(SNLName("I1"));
     SNLScalarTerm* lut2_O = lut2->getScalarTerm(SNLName("O"));
     lut->getInstTerm(lut2_I0)->setNet(model1->getScalarNet(SNLName("in0")));
     lut->getInstTerm(lut2_I1)->setNet(model1->getScalarNet(SNLName("in1")));
     lut->getInstTerm(lut2_O)->setNet(model1->getScalarNet(SNLName("out")));
} 
 {
     //Connect instances in top
     SNLInstance* ins0 = top->getInstance(SNLName("ins0"));
     SNLInstance* ins1 = top->getInstance(SNLName("ins1"));
     SNLScalarTerm* model0_in0 = model0->getScalarTerm(SNLName("in0"));
     SNLScalarTerm* model0_in1 = model0->getScalarTerm(SNLName("in1"));
     SNLScalarTerm* model0_out = model0->getScalarTerm(SNLName("out"));
     SNLScalarTerm* model1_in0 = model1->getScalarTerm(SNLName("in0"));
     SNLScalarTerm* model1_in1 = model1->getScalarTerm(SNLName("in1"));
     SNLScalarTerm* model1_out = model1->getScalarTerm(SNLName("out"));
     ins0->getInstTerm(model0_in0)->setNet(top->getBusNet(SNLName("in"))->getNetBit(0));
     ins0->getInstTerm(model0_in1)->setNet(top->getBusNet(SNLName("in"))->getNetBit(1));
     //Create a net for connection between two instances
     HNLScalarNet* net = HNLScalarNet::create(top, HNLName("net")); 
     ins0->getInstTerm(model0_out)->setNet(net);
     ins1->getInstTerm(model1_in0)->setNet(top->getBusNet(SNLName("in"))->getNetBit(2));
     ins1->getInstTerm(model1_in1)->setNet(net);
     ins1->getInstTerm(model1_out)->setNet(top->getScalarNet(SNLName("out")));

    }

VRLDumper driver;
    driver.setDriveHierarchy(true);
    driver.setDriveBlackBoxes(true);
    boost::filesystem::path dumpVerilogDebugPath(boost::filesystem::path("design.v"));
    driver.drive(top, dumpVerilogDebugPath.string());

    return 0;
}



