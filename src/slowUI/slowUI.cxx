
#include <cassert>
#include <chrono>
#include <iostream>
#include <vector>
#include <sys/select.h>
#include <iomanip>

#include "secApi.h"


int main( int argc, char* argv[]){

  std::clog << "UI: Starting\n" << std::endl;
     
  int keyboardFD = 0;
  SecApi secApi;
  
  int team=9;
  int channel= 5;

  secApi.watch( team, channel );
  
  while( true ) {
    //waitForInput
     struct timeval timeout;
     timeout.tv_sec = 1;
     timeout.tv_usec = 0;
     fd_set fdSet;
     int maxFD=0;
     FD_ZERO(&fdSet);
     FD_SET(keyboardFD, &fdSet); maxFD = (keyboardFD>maxFD) ? keyboardFD : maxFD;
     {
       int sec2uiFD = secApi.getReadFD();
       FD_SET(sec2uiFD, &fdSet); maxFD = (sec2uiFD>maxFD) ? sec2uiFD : maxFD;
     }
     int numSelectFD = select( maxFD+1 , &fdSet , NULL, NULL, &timeout );
     assert( numSelectFD >= 0 );
      
    //processKeyboard
     if ( (numSelectFD > 0) && ( FD_ISSET(keyboardFD, &fdSet) ) ) {
       const int bufSize=128;
       uint8_t keyboardBuf[bufSize];
 
       ssize_t num = read( keyboardFD, keyboardBuf, bufSize );
       if ( num > 0 ) {
         std::vector<uint8_t> msg;

         msg.resize(6);
         uint64_t nowMs = std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::system_clock::now().time_since_epoch() ).count();
         uint64_t d=nowMs;
         for ( int i=5; i >= 0; --i ) {
           msg[i] = ( (uint8_t)( d & 0xFF)  );
           d >>= 8;
         }

         std::chrono::milliseconds nowDuration( nowMs );
         std::chrono::time_point<std::chrono::system_clock> nowTimePoint( nowDuration );
         std::time_t nowCTime = std::chrono::system_clock::to_time_t( nowTimePoint );
         std::tm nowDateTime = *std::localtime( &nowCTime );
           
         for ( int i=0; i<num; i++ ) {
           if ( (  keyboardBuf[i] >= 0x20 ) && ( keyboardBuf[i] <= 0x7E ) ) {
             msg.push_back( keyboardBuf[i] );
           }
         }
         
         if ( msg.size() > 6 ) {
           std::clog << "UI: SendAscii"
                     << " len=" << msg.size()
                     << " team=" << team
                     << " ch=" << channel
                     << " time=" << std::put_time(&nowDateTime, "%T")
                     << std::endl;
         
           secApi.sendAsciiMsg( team, channel, (uint8_t*)msg.data(), msg.size() );
         }
       }
     }
     
    //processSecureProc
     if ( (numSelectFD > 0) && ( FD_ISSET(secApi.getReadFD(), &fdSet) ) ) {
       QMsgUIMessage message{};
       secApi.readMsg( &message );
       
       switch ( message.type ) {
       case QMsgUIReceiveASCIIMessage: {
         std::vector<uint8_t> msgData( message.u.receive_ascii_message.message.data,
                                       message.u.receive_ascii_message.message.data + message.u.receive_ascii_message.message.length );
         if ( msgData.size() > 6 ) {
           uint64_t thenMs = 0;
           for ( int i=0; i<6; i++ ) {
             thenMs <<= 8;
             thenMs |= msgData[i];
           }
                               
           std::chrono::milliseconds thenDuration( thenMs );
           std::chrono::time_point<std::chrono::system_clock> thenTimePoint( thenDuration );
           std::time_t thenCTime = std::chrono::system_clock::to_time_t( thenTimePoint );
           std::tm thenDateTime = *std::localtime( &thenCTime );

           std::clog << "UI: Got AsciiMsg from SecProc:"
                     << " team=" <<   message.u.receive_ascii_message.team_id
                     << " device=" <<   message.u.receive_ascii_message.device_id
                     << " ch=" <<  message.u.receive_ascii_message.channel_id
                     << " time=" << std::put_time(&thenDateTime, "%T")
                     << " val: " << std::string( &(msgData[6]), &(msgData[msgData.size()]) )
                     << std::endl;
         }
       }
         break;
       case QMsgUIInvalid: {
         std::clog << "UI: Got Invalid msg from SecProc: " << std::endl;
       } break;
       default: {
         assert(0);
       }
         break;
       }
      }
  }
  
  return 0;
}
