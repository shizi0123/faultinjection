#include "base/types.hh"
#include "fi/faultq.hh"
#include "fi/regdec_injfault.hh"
#include "fi/fi_system.hh"

using namespace std;

RegisterDecodingInjectedFault::RegisterDecodingInjectedFault( std::ifstream &os)
	:O3CPUInjectedFault(os)
{
	string s;
	os>>s;
	parseRegDec(s);
	fi_system->decodeStageInjectedFaultQueue.insert(this);
	setFaultType(InjectedFault::RegisterDecodingInjectedFault);
}



RegisterDecodingInjectedFault::~RegisterDecodingInjectedFault()
{
}





const char *
RegisterDecodingInjectedFault::description() const
{
    return "RegisterDecodingInjectedFault";
}



void 
RegisterDecodingInjectedFault::dump() const
{
  if (DTRACE(FaultInjection)) {
    std::cout << "===RegisterDecodingInjectedFault::dump()===\n";
    O3CPUInjectedFault::dump();
    std::cout << "\tSrc Or Dst: " << getSrcOrDst() << "\n";
    std::cout << "\tRegister to Change: " << getRegToChange() << "\n";
    std::cout << "\tChange to Reg: " << getChangeToReg() << "\n";
    std::cout << "~==RegisterDecodingInjectedFault::dump()===\n";
  }
}

StaticInstPtr
RegisterDecodingInjectedFault::process(StaticInstPtr staticInst)
{
  int src = (int )staticInst->numSrcRegs() ;
  int dest= (int) staticInst->numDestRegs();
  if (DTRACE(FaultInjection)) {
    std::cout << "===RegisterDecodingInjectedFault::process()===\n";
    std::cout <<"NumSrcRegs : "<< src<<  " NumDestRegs : "<<dest<<"\n";
    std::cout <<"Instruction Type : "<<staticInst->getName()<<"\n";;
    dump();
  }
  
 
  if (getSrcOrDst() == RegisterDecodingInjectedFault::SrcRegisterInjectedFault) {
    int rTc = getRegToChange();
      if (rTc < staticInst->numSrcRegs()) {
	staticInst->_srcRegIdx[rTc] = getChangeToReg();
      }
      else{
	DPRINTF(FaultInjection,"Wrong number of source reg\n");
      }
  }
  else {
    int rTc = getRegToChange();
      if (rTc < staticInst->numDestRegs()) {
	staticInst->_destRegIdx[rTc] = getChangeToReg();
      }
      else{
	DPRINTF(FaultInjection,"Wrong number of Destination reg \n");
      }
  }  

  
  check4reschedule();

  if (DTRACE(FaultInjection)) {
    std::cout << "~==RegisterDecodingInjectedFault::process()===\n";
  }
  return staticInst;
}

int
RegisterDecodingInjectedFault::parseRegDec(std::string s)
{
  if (DTRACE(FaultInjection)) {
    std::cout << "RegisterDecodingInjecteFault::parseRegDec()\n";
  }
  size_t pos;

  if (s.compare(0,3,"Src",0,3) == 0) {
    setSrcOrDst(RegisterDecodingInjectedFault::SrcRegisterInjectedFault);

    std::string s2 = s.substr(4);
    pos = s2.find_first_of(":");

    setRegToChange(s2.substr(0,pos));
    setChangeToReg(s2.substr(pos+1));
  }
  else if (s.compare(0,3,"Dst",0,3) == 0) {
    setSrcOrDst(RegisterDecodingInjectedFault::DstRegisterInjectedFault);

    std::string s2 = s.substr(4);
    pos = s2.find_first_of(":");

    setRegToChange(s2.substr(0,pos));
    setChangeToReg(s2.substr(pos+1));
    
  }
  else {
    std::cout << "RegisterDecodingInjecteFault::parseRegDec(): " << s << "\n";
    assert(0);
    return 1;
  }
  
  return 0;
}
