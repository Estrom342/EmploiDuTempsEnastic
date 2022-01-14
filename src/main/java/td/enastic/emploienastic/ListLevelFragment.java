package td.enastic.emploienastic;

import android.os.Bundle;
import androidx.annotation.NonNull;
import androidx.fragment.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;
import td.enastic.emploienastic.databinding.FragmentListLevelBinding;
import td.enastic.emploienastic.databinding.LevelViewBinding;

import java.util.ArrayList;

public class ListLevelFragment extends Fragment {

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    private FragmentListLevelBinding ui;
    private LevelAdapter levelAdapter;
    private OnShowTimeTableListener listener;

    public ListLevelFragment() {
    }

    public ListLevelFragment(OnShowTimeTableListener listener) {
        levelAdapter = new LevelAdapter(new ArrayList<>());
        this.listener = listener;
        levelAdapter.setListener(this.listener);
    }

    public OnShowTimeTableListener getListener() {
        return listener;
    }

    public void setListener(OnShowTimeTableListener listener) {
        this.listener = listener;
    }

    public void setLsLevel(ArrayList<Level> lsLevel){
        levelAdapter.setLsLevel(lsLevel);
    }

    public ArrayList<Level> getLsLevel(){
        return levelAdapter.getLsLevel();
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        ui = FragmentListLevelBinding.inflate(inflater, container, false);
        ui.levelFragmentRecyclerView.setAdapter(levelAdapter);
        ui.levelFragmentRecyclerView.setLayoutManager(new LinearLayoutManager(container.getContext(),
                RecyclerView.VERTICAL, false));
        return ui.getRoot();
    }


    //////////////////////////////////////////////////////////////////////////////////
    ////              Level adapter                                     /////////////
    //////////////////////////////////////////////////////////////////////////////////

    static class LevelAdapter extends RecyclerView.Adapter<LevelHolder> {

        ArrayList<Level> lsLevel;
        OnShowTimeTableListener listener;

        public LevelAdapter(ArrayList<Level> lsLevel){
            this.lsLevel = lsLevel;
        }

        public ArrayList<Level> getLsLevel() {
            return lsLevel;
        }

        public void setLsLevel(ArrayList<Level> lsLevel) {
            this.lsLevel = lsLevel;
        }

        public OnShowTimeTableListener getListener() {
            return listener;
        }

        public void setListener(OnShowTimeTableListener listener) {
            this.listener = listener;
        }

        @NonNull
        @Override
        public LevelHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
            LevelViewBinding ui = LevelViewBinding.inflate(LayoutInflater.from(parent.getContext()),
                    parent, false);
            LevelHolder levelHolder = new LevelHolder(ui);
            //levelHolder.setListener(listener);
            ui.getRoot().setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    listener.setCurrentIndex(levelHolder.level.index);
                }
            });
            return levelHolder;
        }

        @Override
        public void onBindViewHolder(@NonNull LevelHolder holder, int position) {
            holder.setLevel(lsLevel.get(position));
        }

        @Override
        public int getItemCount() {
            return lsLevel.size();
        }
    }
    /////////////////////////////////////////////////////////////////////////////////
    ////            Level view holder                           //////////////
    /////////////////////////////////////////////////////////////////////////////////
    public static class LevelHolder extends RecyclerView.ViewHolder{
        OnShowTimeTableListener listener;
        private Level level;
        LevelViewBinding ui;
        public LevelHolder(LevelViewBinding ui) {
            super(ui.getRoot());
            this.ui = ui;
        }

        public OnShowTimeTableListener getListener() {
            return listener;
        }

        public void setListener(OnShowTimeTableListener listener) {
            this.listener = listener;
        }

        public Level getLevel() {
            return level;
        }

        public void setLevel(Level level) {
            this.level = level;
            ui.levelViewClassName.setText(level.className);
            ui.levelViewName.setText(level.name);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////
    ////                    Level                                       ///////////////
    ///////////////////////////////////////////////////////////////////////////////////
    public static class Level {
        private String name;
        private String className;
        int index;

        public Level(String name, String className, int index) {
            this.name = name;
            this.className = className;
            this.index = index;
        }

        public String getName() {
            return name;
        }

        public void setName(String name) {
            this.name = name;
        }

        public String getClassName() {
            return className;
        }

        public void setClassName(String className) {
            this.className = className;
        }

        public int getIndex() {
            return index;
        }

        public void setIndex(int index) {
            this.index = index;
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////

    public static interface OnShowTimeTableListener {
        public void setCurrentIndex(int index);
    }
}