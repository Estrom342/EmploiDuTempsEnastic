package td.enastic.emploienastic;

import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import androidx.annotation.NonNull;
import androidx.fragment.app.Fragment;
import td.enastic.emploienastic.databinding.FragmentTimetableBinding;
import td.enastic.emploienastic.model.*;

import java.util.ArrayList;

public class TimeTableFragment extends Fragment{

    public FragmentTimetableBinding ui;
    private ArrayList<EmploiDuTemp> lsTimeTables = new ArrayList<>();
    private int currentIndex;

    public TimeTableFragment() {
        currentIndex = 0;
        Log.i("DEBUGShedule", "On construct Shedule Fragment");
    }

    public int getCurrentIndex() {
        return currentIndex;
    }

    public void setCurrentIndex(int currentIndex) {
        this.currentIndex = currentIndex;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        Log.i("DEBUGShedule", "Oncreate Shedule Fragment "+ (savedInstanceState == null));
        super.onCreate(savedInstanceState);
    }

    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        Log.i("DEBUGShedule", "onCreateView Shedule Fragment "+ currentIndex);
        ui = FragmentTimetableBinding.inflate(LayoutInflater.from(container.getContext()), container, false);

        ui.timeTableBtnNext.setOnClickListener(this::next);
        ui.timeTableBtnPreview.setOnClickListener(this::preview);
        loadTimeTable(currentIndex);
        return ui.getRoot();
    }

    public void next(View v){
        loadTimeTable(++currentIndex);
        Log.i("MYDEBUG", "Next");
    }
    public void preview(View v){
        loadTimeTable(--currentIndex);
        Log.i("MYDEBUG", "Preview");
    }

    public void loadTimeTable(int index){
        if(lsTimeTables.isEmpty()){
            ui.idSheduleWebView.loadData(Constant.TIMETABLE_NOT_AVAIBLE_TEXT, "text/html", "UTF-8");
            return;
        }
        try {
            EmploiDuTemp em = lsTimeTables.get(index);
            ui.idSheduleWebView.loadData(em.toHtml(), "text/html", "UTF-8");
            currentIndex = index;
        } catch (Exception e) {
            if(currentIndex > lsTimeTables.size() -1){
                currentIndex = lsTimeTables.size() -1;
            }if(currentIndex<0){
                currentIndex=0;
            }
        }
    }

    public void timeTableAvailable(EmploiDuTemp em) {
        for (EmploiDuTemp ept: lsTimeTables){
            if(ept.niveau().equals(em.niveau())){
                ept.setDateDebut(em.dateDebut());
                ept.setDateFin(em.dateFin());
                ept.setLsHoraire(em.lsHoraire());
                ept.setLsMatieres(em.lsMatieres());
                loadTimeTable(currentIndex);
                return;
            }
        }
        lsTimeTables.add(em);
        loadTimeTable(currentIndex);
    }

    public void signatureAvailable(String level, String nomSalle, VDate dateDebut, VDate dateFin, byte[] signature) {

    }

    public void setLsTimeTable(ArrayList<EmploiDuTemp> lsTimeTables) {
        this.lsTimeTables = lsTimeTables;
        currentIndex = 0;
        loadTimeTable(currentIndex);
    }
}