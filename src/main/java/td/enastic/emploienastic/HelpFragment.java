package td.enastic.emploienastic;

import android.annotation.SuppressLint;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Toast;
import androidx.fragment.app.Fragment;
import td.enastic.emploienastic.databinding.FragmentHelpBinding;
import td.enastic.emploienastic.model.Constant;

public class HelpFragment extends Fragment {

    FragmentHelpBinding ui;

    public HelpFragment() {
        // Required empty public constructor
    }
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
            }

    @SuppressLint("ClickableViewAccessibility")
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        ui = FragmentHelpBinding.inflate(inflater, container, false);
        ui.idHelpView.getSettings().setSupportZoom(true);
        ui.idHelpView.loadData(Constant.HELP_TEXT, "text/html", "UTF-8");
        return ui.getRoot();
    }
}