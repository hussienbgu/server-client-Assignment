package bgu.spl.net.impl.stomp;
import bgu.spl.net.api.StompMessagingProtocol;
import bgu.spl.net.srv.Connections;
import java.util.concurrent.atomic.AtomicInteger;
import java.lang.String;

public class StompProtocolImpl<T> implements StompMessagingProtocol<T>{
    private boolean shouldTerminate=false;
    private int ConnectionID;
    private Connections<T> Connections;
    private SharedProtocolData data;
    private String userName="";
    private AtomicInteger msgid;
    public StompProtocolImpl(SharedProtocolData dataManger){
        this.data=dataManger;
        this.msgid=new AtomicInteger(0);
    }
    @Override
    public void start(int connectionId,Connections<T> connections) {
        this.Connections=connections;
        this.ConnectionID=connectionId;
    }
    @Override
    public void process(T message){
        Frame MainMsg = new Frame((String) message);
        String command=MainMsg.getComand();
        switch (command){
            case "CONNECT":{
                login(MainMsg);
            }break;
            case "SUBSCRIBE":{
                subsribe(MainMsg);
            }break;
            case "SEND":{
                 send(MainMsg);
            }break;
            case "DISCONNECT":{
                logout(MainMsg);
            }break;
            case "UNSUBSCRIBE":{
                exit(MainMsg);
            }
        }
    }
    @Override
    public boolean shouldTerminate() {
        return shouldTerminate;
    }
    public void login(Frame Main) {
        String username = Main.getHeaderValue("login");
        this.userName = username;
        String passcode = Main.getHeaderValue("passcode");
        String version = Main.getHeaderValue("accept-version");
        if (!data.getReg().containsKey(username)) {
            data.AddNewUser(username, passcode, this.ConnectionID);
            Frame reply = new Frame();
            reply.setComand("CONNECTED");
            reply.setHeaders("version", version);
            this.Connections.send(this.ConnectionID, (T) reply.getPureString());
        }
        else if (data.getActUsers().containsKey(username) && data.getReg().get(username).equals(passcode)) {
            msgid.incrementAndGet();
            Frame Error = new Frame();
            Error.setComand("ERROR");
            Error.setHeaders("receipt-id", msgid.toString());
            Error.setHeaders("message", "user already logged in");
            Error.setFrame("-----\n" + Main.getPureString() + "-----\n" + "User already logged in\n");
            this.Connections.send(this.ConnectionID, (T) Error.getPureString());
            this.Connections.disconnect(this.ConnectionID);
            this.shouldTerminate = true;
        } else if (data.getReg().containsKey(username) && !data.getReg().get(username).equals(passcode)) {
            msgid.incrementAndGet();
            Frame Error = new Frame();
            Error.setComand("ERROR");
            Error.setHeaders("receipt-id", msgid.toString());
            Error.setHeaders("message", "wrong password");
            Error.setFrame("-----\n" + Main.getPureString() + "-----\n" + "Wrong password\n");
            this.Connections.send(this.ConnectionID, (T) Error.getPureString());
            this.Connections.disconnect(this.ConnectionID);
            this.shouldTerminate = true;
        }
        else {
            data.getActUsers().put(username, this.ConnectionID);
            Frame reply = new Frame();
            reply.setComand("CONNECTED");
            reply.setHeaders("version", version);
            this.Connections.send(this.ConnectionID, (T) reply.getPureString());
        }
    }
    private void subsribe(Frame Main) {//Done
        String genre = Main.getHeaderValue("destination");
        String subId = Main.getHeaderValue("id");
        String reciptId = Main.getHeaderValue("receipt");
        this.data.subscribe(genre,this.userName,subId);
        Frame reply=new Frame();
        reply.setComand("RECEIPT");
        reply.setHeaders("receipt-id",reciptId);
        this.Connections.send(this.ConnectionID, (T) reply.getPureString());
    }
    private void send(Frame Main){
        String body= Main.getFrame();
        String genre = Main.getHeaderValue("destination");
        Integer q = msgid.incrementAndGet();
        if(data.getActUsers().keySet()!=null&&!data.getActUsers().keySet().isEmpty()){
         for (String username : data.getActUsers().keySet()) {
            for (int k = 0; data.getUsersinTheTopic().get(genre)!=null&&k < data.getUsersinTheTopic().get(genre).size(); k++) {
                if (data.getUsersinTheTopic().get(genre).get(k)[0].equals(username)) {
                    Frame tosend = new Frame();
                    tosend.setComand("MESSAGE");
                    tosend.setHeaders("subscription", data.getUsersinTheTopic().get(genre).get(k)[1].toString());
                    tosend.setHeaders("Message-id", "" + q);
                    tosend.setHeaders("destination", genre);
                    tosend.setFrame(body);
                    this.Connections.send(data.getActUsers().get(username), (T) tosend.getPureString());
                }
            }
            }
        }
    }
    private void exit(Frame Main){
        String id=Main.getHeaderValue("id");
        String receipt=Main.getHeaderValue("receipt");
        String genre="";
        for (String topic:this.data.getUsersinTheTopic().keySet()) {
            for (int j = 0; j <this.data.getUsersinTheTopic().get(topic).size(); j++) {
                if (this.data.getUsersinTheTopic().get(topic).get(j)[0].equals(userName)) {
                    genre = topic;
                    this.data.getUsersinTheTopic().get(genre).remove(j);
                }
            }
        }
        Frame reply=new Frame();
        reply.setComand("RECEIPT");
        reply.setHeaders("receipt-id",receipt);
        this.Connections.send(this.ConnectionID, (T) reply.getPureString());
    }
    private void logout(Frame Main){
            String receipt=Main.getHeaderValue("receipt");
            Frame reply=new Frame();
            reply.setComand("RECEIPT");
            reply.setHeaders("receipt-id",receipt);
            this.Connections.send(this.ConnectionID, (T) reply.getPureString());
            data.removeUser(userName);
            this.userName = "";
            this.Connections.disconnect(this.ConnectionID);
            this.shouldTerminate=true;
    }
}

