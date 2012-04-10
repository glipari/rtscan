#include <cmath>

#include "medf_rta.hpp"
#include <models/task_ex.hpp>

namespace Scan {

    using namespace std;

    class HasSlack {
    public:
        double slack;
    };

    typedef TaskEx<HasSlack> MEDF_Task;

    double interfcompl_i_edf(int t_old, int i, int k, const vector<MEDF_Task>& tset) 
    {
        int n_i_edf = 0;
        int n_i = 0;
        double a,b,c;
        double interf=0;
        
        // Definitions
        n_i_edf = max(0,int(floor((tset[k].get_dline() - tset[i].slack)/tset[i].get_period())));
        n_i = int (floor((t_old + tset[i].get_dline() - tset[i].get_wcet() - tset[i].slack)/tset[i].get_period()));
        
        // Compute the interference
        a = n_i*tset[i].get_wcet() + min(tset[i].get_wcet(),t_old + tset[i].get_dline() - tset[i].get_wcet() - tset[i].slack - n_i * tset[i].get_period());
        b = n_i_edf*tset[i].get_wcet() + min(tset[i].get_wcet(), max(0.0,tset[k].get_dline() - tset[i].slack - n_i_edf*tset[i].get_period()));
        c = max(0.0, t_old - ceil(t_old/tset[k].get_period())*tset[k].get_wcet()) + 1; 
        
        interf = min(a, min(b,c));

        return interf;
}


    bool rta_compl_edf(int n_proc, vector<Task>& v)
    {
        int t_new, t_old;
        double interf_tot = 0;

        bool stop = false;
        bool feasible = false;
        
        vector<MEDF_Task> tset(v.begin(), v.end());

        // Azzera gli slack
        for(int k=0; k<tset.size(); k++)
            tset[k].slack = 0;
        
        /* task ordinati e slack settato */

        // Ciclo ricorsivo su tutti i task
        while (!stop && !feasible) {
            // Inizializza i booleani
            stop=true;
            feasible = true;

            // Ciclo t_new[k]
            for(int k = tset.size()-1; k >= 0; k--) {
                interf_tot = 0;

                // Step 1 = Inizializzazioni
                t_old = 0;
                t_new = int(tset[k].get_wcet());   //e_tot

                // Cyclic Passage 1 = t_new secondo RTA
                while(t_new != t_old && t_new <= tset[k].get_dline()){
                    interf_tot = 0;
                    t_old = t_new;
                    
                    // Calcola l'interferenza aumentata
                    for(int i = 0; i < tset.size(); i++){
                        if (i != k)
                            interf_tot += interfcompl_i_edf(t_old, i, k, tset);
                    }
                    // Calcola il nuovo t_new secondo l'interferenza
                    t_new = int( ceil(t_old/tset[k].get_period())*tset[k].get_wcet() + floor(interf_tot/n_proc));
                }
                
                // Controllo finale schedulabilità e/o modifica
                if (t_new > tset[k].get_dline())
                    feasible = false;
                
                if (tset[k].slack < tset[k].get_dline() - t_new) {
                    tset[k].slack = tset[k].get_dline() - t_new;
                    stop=false;
  		}
                
            }
            // Fine ciclo t_new[k]
        }
        // Fine ciclo ricorsivo su tutti i task
        
        if (!feasible)
            return false;
        else
            return true;
    }

    
};
