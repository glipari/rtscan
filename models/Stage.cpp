#include <models/Stage.hpp>

using namespace std;
using namespace Scan;
namespace Scan
{

    Stage::Stage(): index_Pipe(-1),index_Task_In_Pipe(-1)
    {
        task= new Task();
    }
    Stage::Stage(Task&t, int i, int j): index_Pipe(i),index_Task_In_Pipe(j)
    {

        task = new Task(t.get_wcet(), t.get_dline(), t.get_period());
    }

   Stage:: Stage( const Stage &r)
    {
        index_Pipe=r.index_Pipe;

        const Task t= r.get_Task();

        task = new Task(t.get_wcet(), t.get_dline(), t.get_period()) ;

        index_Task_In_Pipe=r.index_Task_In_Pipe;
    }

    int Stage::get_Index_Pipe()
    {
        return index_Pipe;
    }

    int Stage::get_Index_In_Pipe()
    {
        return index_Task_In_Pipe;
    }

    const Task& Stage::get_Task() const
    {
        return *task;
    }
    bool Stage::operator==(const Stage &other) const
    {
        if(index_Pipe==other.index_Pipe&& index_Task_In_Pipe==other.index_Task_In_Pipe)
            return true;
        return false;
    }
    bool Stage::equal(int indexP, int position)
    {
        if(index_Pipe==indexP && index_Task_In_Pipe==position)
            return true;
        return false;
    }

    Stage& Stage::operator=(const Stage &r)
    {
        index_Pipe=r.index_Pipe;
        if(index_Pipe!=-1)
        {
            const Task t= r.get_Task();
            task->set_period( t.get_period());
            task->set_wcet( t.get_wcet());

            task->set_dline(t.get_dline() ) ;
        }

        index_Task_In_Pipe=r.index_Task_In_Pipe;
        return *this;
    }

    std::ostream &operator<<(ostream &s, Stage &r)
{
	    s<<"Index  pipe: "<<r.get_Index_Pipe()<<endl;
        s<<"Index in pipe: "<<r.get_Index_In_Pipe()<<endl;
        s<<"wcet task in Stage: "<<r.get_Task().get_wcet()<<endl;
		s << "-------------------" << endl;
		return s;
}


}
