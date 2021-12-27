package bgu.spl.net.srv;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.atomic.AtomicInteger;
public class ConnectionsImpl<T> implements Connections<T> {
    ConcurrentHashMap<Integer,ConnectionHandler<T>> actClients;
    AtomicInteger connId = new AtomicInteger(0);
    public ConnectionsImpl(){
        actClients = new ConcurrentHashMap<>();
    }
    @Override
    public boolean send(int connectionId, T msg) {
        if(actClients.get(connectionId) !=null) {
            actClients.get(connectionId).send(msg);
            return true;
        }
        return false;
    }

    @Override
    public void send(String channel, T msg) {

    }

    @Override
    public void disconnect(int connectionId) {
        this.actClients.remove(connectionId);
    }

    public synchronized void connect(ConnectionHandler<T> handler){
        actClients.put(connId.get(),handler);
        handler.setConnectionId(connId.getAndIncrement());
    }

}