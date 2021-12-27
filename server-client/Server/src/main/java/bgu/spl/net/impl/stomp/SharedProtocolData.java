package bgu.spl.net.impl.stomp;
import java.util.concurrent.*;

public class SharedProtocolData {
    private ConcurrentHashMap<String,Integer> actUsers;
    private ConcurrentHashMap<String,CopyOnWriteArrayList<Object[]>> usersinTheTopic;
    private ConcurrentHashMap<String,String> reg;
    public SharedProtocolData(){
        this.actUsers = new ConcurrentHashMap<>();
        this.reg = new ConcurrentHashMap<>();
        this.usersinTheTopic=new ConcurrentHashMap<>();
    }
    public void AddNewUser(String name,String password,int userId){
        this.reg.put(name,password);
        this.actUsers.put(name,userId);
    }
    public void subscribe(String genre,String username,String subid){
        this.usersinTheTopic.putIfAbsent(genre,new CopyOnWriteArrayList<>());
        Object [] x=new Object[2];
        x[0]=username;
        x[1]=subid;
        this.usersinTheTopic.get(genre).add(x);
    }
    public ConcurrentHashMap<String, CopyOnWriteArrayList<Object[]>> getUsersinTheTopic() {
        return usersinTheTopic;
    }
    public ConcurrentHashMap<String, Integer> getActUsers() {
        return actUsers;
    }
    public ConcurrentHashMap<String, String> getReg() {
        return reg;
    }
    public void removeUser(String Username){
        this.actUsers.remove(Username);
        if(!this.usersinTheTopic.isEmpty())
            for (int i = 0; i <this.usersinTheTopic.size() ; i++) {
                if(this.usersinTheTopic.get(i)!=null&&!this.usersinTheTopic.get(i).isEmpty())
                    this.usersinTheTopic.get(i).removeIf(x -> x[0].equals(Username));
        }
    }

}