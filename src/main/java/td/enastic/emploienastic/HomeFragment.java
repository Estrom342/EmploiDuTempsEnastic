package td.enastic.emploienastic;

import android.os.Bundle;
import androidx.fragment.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import td.enastic.emploienastic.databinding.FragmentHomeBinding;

public class HomeFragment extends Fragment {

    FragmentHomeBinding ui;
    MainActivity context = null;

    public HomeFragment() {
        // Required empty public constructor
    }

    public HomeFragment(MainActivity context) {
        // Required empty public constructor
        this.context = context;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        ui = FragmentHomeBinding.inflate(inflater, container, false);
        ui.button.setOnClickListener(v -> context.showShedule());
        return ui.getRoot();
    }
}