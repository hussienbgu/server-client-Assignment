package bgu.spl.net.srv;

import bgu.spl.net.api.MessageEncoderDecoder;
import bgu.spl.net.api.MessagingProtocol;
import bgu.spl.net.api.StompMessagingProtocol;
import bgu.spl.net.impl.stomp.StompEncoderDecoderImpl;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.IOException;
import java.net.Socket;

public class BlockingConnectionHandler<T> implements Runnable, ConnectionHandler<T> {
    private final StompMessagingProtocol<T> protocol;
    private final StompEncoderDecoderImpl<T> encdec;
    private final Socket sock;
    private BufferedInputStream in;
    private BufferedOutputStream out;
    private volatile boolean connected = true;
    int conId = 0;
    ConnectionsImpl<T> connections;
    public BlockingConnectionHandler(Socket sock, StompEncoderDecoderImpl<T> reader, StompMessagingProtocol<T> protocol,ConnectionsImpl<T> actclient) {
        this.sock = sock;
        this.encdec = reader;
        this.protocol = protocol;
        this.connections=actclient;
    }

    @Override
    public void run() {
        try (Socket sock = this.sock) { //just for automatic closing
            int read;
            protocol.start(conId,connections);
            in = new BufferedInputStream(sock.getInputStream());
            out = new BufferedOutputStream(sock.getOutputStream());
            while (!protocol.shouldTerminate() && connected && (read = in.read()) >= 0) {
                T nextMessage = encdec.decodeNextByte((byte) read);
                if (nextMessage != null) {
                    protocol.process(nextMessage);
                }
            }
            if(protocol.shouldTerminate())
                this.close();
        } catch (IOException ex) {
            ex.printStackTrace();
        }

    }
    @Override
    public void close() throws IOException {
        connected = false;
        sock.close();
    }
    @Override
    public void send(T msg) {
        if (msg != null) {
            try {
                out.write(encdec.encode(msg));
                out.flush();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }
    public void setConnectionId(int i) {
        this.conId = i;

    }
}
