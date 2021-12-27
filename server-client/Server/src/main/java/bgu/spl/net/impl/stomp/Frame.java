package bgu.spl.net.impl.stomp;
import java.io.*;
import java.util.*;

public class Frame {
    private String Stompcommand;
    private List<String> headers=new LinkedList<>();
    private String body;

    public Frame(){};
    public Frame(String FrameAsString){
            try {
                BufferedReader reader = null;
                reader = new BufferedReader(new StringReader(FrameAsString));
                Stompcommand = reader.readLine();// Stomp command
                String hdr = null;
                hdr = reader.readLine();//
                while (!hdr.equals("")) {// to check
                    headers.add(hdr);
                    hdr = reader.readLine();
                }
                body = reader.readLine();
                body = body == null || body.equals('\u0000') ? null : fillBody(body, reader);
            }
            catch (Exception e){
            }
        }
        private String fillBody(String body, BufferedReader reader) throws IOException {
            StringBuilder builder = new StringBuilder(trimEndOfMsg(body));
            String s = reader.readLine();
            while (!s.equals("\u0000")) {
                builder.append(" "+trimEndOfMsg(s));
                s = reader.readLine();
            }
            return builder.toString();
        }private String trimEndOfMsg(String s) {
            String trimmed = s;
            if (s.contains("\u0000")) {
                int idx = s.indexOf("\u0000");
                trimmed = s.substring(0, idx);
            }
            return trimmed;
    }
    public void setComand(String m){
        this.Stompcommand=m;
    }
    public void setFrame(String m){
        this.body=m;
    }
    public void setHeaders(String m, String e){
        this.headers.add(m+":"+e);

    }
    public String getComand()
    {
        return this.Stompcommand;
    }
    public String getHeaderValue(String key) {
        for (int i = 0; i < headers.size(); i++) {
            if (headers.get(i).split(":")[0].equals(key))
                if(headers.get(i).split(":").length<2)
                    return "";
                else
                return headers.get(i).split(":")[1];
        }
        return "";
    }
    public void setHeaderValue(String key, String value){
        headers.add(key+":"+value);
    }
    public String getFrame()
    {
        return body;
    }
    public String getString (){

        String Headers = new String();
        for (String s : headers){
            Headers = Headers + s +'\n';
        }
        Headers=Headers+'\n';
        if (body == null) {
            return (Stompcommand+'\n'+Headers+'\n'+'\u0000');

        }
        return (Stompcommand+'\n'+Headers+body+'\n'+'\u0000');


    }
    public String getPureString (){

        String Headers = new String();
        for (String s : headers){
            Headers = Headers +s+'\n';
        }
        Headers=Headers+'\n';
        if (body == null) {
            return (Stompcommand+'\n'+Headers);

        }
        return (Stompcommand+'\n'+Headers+body+'\n');
    }
    public List<String> getHeaders() {
        return headers;
    }
}