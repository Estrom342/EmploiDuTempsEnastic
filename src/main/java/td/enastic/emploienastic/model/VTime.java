package td.enastic.emploienastic.model;

import android.util.Log;

import java.util.Calendar;

public class VTime {

    private int m_hour;
    private int m_minute;
    private int m_second;
    private int m_ms;

    public VTime(){
        Calendar c = Calendar.getInstance();
        m_hour = c.get(Calendar.HOUR_OF_DAY) % 60;
        m_minute = c.get(Calendar.MINUTE) % 60;
        m_second = c.get(Calendar.SECOND) % 60;
        m_ms = c.get(Calendar.MILLISECOND);
    }
    public VTime(int h, int m, int s, int ms){
        m_hour = h; m_minute = m; m_second = s;
        m_ms = ms;
    }

    public int getMs() {
        return m_ms;
    }
    public boolean isValid(){
        return 0<= m_hour && m_hour <= 60 && 0 <= m_minute && m_minute<=60 &&
                0<= m_second && m_second <= 60;
    }

    public static VTime now(){
        return new VTime();
    }

    public int hour(){
        return m_hour;
    }
    public int minute(){
        return m_minute;
    }
    public int second(){
        return m_second;
    }

    public VTime addSecond(int second){
        Integer s = m_hour * 3600 + m_minute*60 + m_second + second;
        m_hour= s/360; s = s - s/3600;
        m_minute = s/60; s = s - s/60;
        m_second = s;
        return this;
    }

    @Override
    public String toString() {
        return  m_hour + (m_hour > 10 ? ":h":"0h:")+
                m_minute + (m_minute > 10 ? ":m" : ":0m");
    }

    public static VTime fromString(String string){
        int h = 0, m = 0, s=0, ms=0;
        try {
            String[] tab = string.split("\n")[0].split(":");
            h= Integer.parseInt(tab[0]);
            m = Integer.parseInt(tab[1]);
            s = tab.length>= 3 ? Integer.parseInt(tab[2]) : 0;
            ms = tab.length >= 4 ? Integer.parseInt(tab[3]) : 0;
        }catch (IndexOutOfBoundsException | NumberFormatException e){
            Log.i("MYDEBUG", "Error at VTime.fromString: " +string+ e.getClass().getName()+" "+e.getMessage());
        }
        return new VTime(h, m, s, ms);
    }
    public String toSqlFormat(){
        return  (m_hour >= 10 ? m_hour+":":"0"+m_hour+":")+
                (m_minute >= 10 ? m_minute+ ":" : "0"+m_minute+":")+
                (m_second >= 10 ? m_second+ "" : "0"+m_second)+"\n";
    }

    public enum FORMAT {
        SQL, STRING;
    }
}
