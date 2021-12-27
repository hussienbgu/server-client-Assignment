package bgu.spl.net.impl.stomp;

import bgu.spl.net.srv.Server;

public class StompServer {
    public static void main(String[] args) {
        SharedProtocolData data=new SharedProtocolData();
     //   Server.threadPerClient(
       //      7777, //port
         //() -> new StompProtocolImpl<>(data), //Stompprotocol factory
          //      StompEncoderDecoderImpl::new //Stomp encoder decoder factory
        //).serve();
        Server.reactor(
                Runtime.getRuntime().availableProcessors(),
                7777, //port
                () ->  new StompProtocolImpl<>(data), //protocol factory
                StompEncoderDecoderImpl::new //message encoder decoder factory
        ).serve();

    }
}
