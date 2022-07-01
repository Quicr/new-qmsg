
#include <assert.h>
#include <iostream>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

#include "secApi.h"


int main( int argc, char* argv[]){

  std::clog <<   "NET: Starting netProc" << std::endl;
  SecApi secApi;
  
  while( true ) {
    //waitForInput
    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    fd_set fdSet;
    int maxFD=0;
    FD_ZERO(&fdSet);
    {
      int sec2netFD = secApi.getReadFD();
      FD_SET(sec2netFD, &fdSet); maxFD = (sec2netFD>maxFD) ? sec2netFD : maxFD;
    }
    int numSelectFD = select( maxFD+1 , &fdSet , NULL, NULL, &timeout );
    assert( numSelectFD >= 0 );

    
    // processs secProc
    if ( (numSelectFD > 0) && ( FD_ISSET( secApi.getReadFD(), &fdSet) ) ) {
       std::clog <<   "NET: Reading from SecProc" << std::endl;
    
      QMsgNetMessage message{};
      secApi.readMsg( &message );

      switch ( message.type ) {
      case QMsgNetWatchDevices:
        {
          std::clog << "NET: Got watch from SecProc:"
                    << " team=" <<    message.u.watch_devices.team_id
                  << " ch= " <<   message.u.watch_devices.channel_id
              << std::endl;
           // get a local copy before memory is invalid
         uint16_t* ptr =  message.u.watch_devices.device_list.device_list;
         uint32_t len =   message.u.watch_devices.device_list.num_devices;
         std::vector<uint16_t> devList( ptr, ptr+len );
         std::clog << "   dev: ";
         for ( const auto& dev : devList ) {  std::clog << dev << " "; }
         std::clog << std::endl;
        
        } 
        break;
      case QMsgNetSendASCIIMessage:

        std::clog << "NET: Got SendAsciiMsg from SecProc: "
          << " team=" <<   message.u.send_ascii_message.team_id
          //<< " device=" <<   message.u.send_ascii_message.device_id
          << " ch= " <<  message.u.send_ascii_message.channel_id
          << " val: " << std::string(  (char*)message.u.send_ascii_message.message.data,
                                       message.u.send_ascii_message.message.length )
          << std::endl;

#if 1
          secApi.recvAsciiMsg(  message.u.send_ascii_message.team_id,
                                0x7, // TODO message.u.send_ascii_message.device_id,
                    message.u.send_ascii_message.channel_id,
                    message.u.send_ascii_message.message.data,
                    message.u.send_ascii_message.message.length );
#endif    
        break;
      case QMsgNetInvalid:
         std::clog << "NET: Got Invalid msg from SecProc: " << std::endl;
         break;
      default:
        assert(0);
      }
    }
  }
  
  return 0;
}
