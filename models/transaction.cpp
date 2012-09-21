#include <algorithm>

#include <models/transaction.hpp>
#include <common/exceptions.hpp>


using namespace std;

namespace Scan {
	Transaction::Transaction() : name(), period(0), end_to_end_D(0)
	{
	}

	Transaction::Transaction(int p, double d) : name("tr"), period(p), end_to_end_D(d)
	{
	}

	string Transaction::get_name()
	{
		return name;
	}


	void Transaction::setid(int i)
	{
		id = i;
	}

	int Transaction::getid()
	{
		return id;
	}

	void Transaction::add_task(const Task &t, int node)
	{
		//tasks.push_back(t);
		tasks += t;
		allocated.push_back(node);
		int size = offsets.size()-1;
		double base = size > -1 ? offsets[size] + tasks[size].get_dline() : 0;
		offsets.push_back(base);
	}


	void Transaction::add_task(double c, double d, int node)
	{
			Task t(c, d, period);
			add_task(t, node);
	}

	int Transaction::get_period() const
	{
			return period;
	}

	void Transaction::set_period(const int p)
	{
		period = p;
	}

	double Transaction::get_dline() const
	{
			return end_to_end_D;
	}

	void Transaction::set_dline(const double dl)
	{
		end_to_end_D = dl;
	}

	Task &Transaction::get_task(unsigned int i)
	{
		if (i>=tasks.size()) THROW_EXC(IndexOutOfBound, "Transaction::get_task();");
		return tasks[i];
	}

	const Task Transaction::get_const_task(unsigned int i) const
	{
		if (i>=tasks.size()) THROW_EXC(IndexOutOfBound, "Transaction::get_task();");
		return tasks.at(i);
	}

	int Transaction::get_task_node(unsigned int i) const
	{
		if (i>=tasks.size()) THROW_EXC(IndexOutOfBound, "Transaction::get_task_node();");

		return allocated[i];
	}

	double Transaction::get_task_offset(unsigned int i)
	{
		if (i>=tasks.size()) THROW_EXC(IndexOutOfBound, "Transaction::get_task_node();");

		return offsets[i];
	}

	int Transaction::get_num_tasks() const
	{
		return tasks.size();
	}

	void Transaction::normalize_dlines()
	{
		double sum = 0;
		for (unsigned int i=0; i<tasks.size(); i++)
			sum += tasks[i].get_dline();

		sum = end_to_end_D / sum;

		double offset = 0;
		for (unsigned int i=0; i<tasks.size(); i++) {
			double tmp_dl = tasks[i].get_dline()*sum;
			tasks[i].set_dline(tmp_dl);
			offsets[i] = offset;
			offset += tmp_dl;
		}
	}


	void Transaction::proportional_dlines()
	{
		for (unsigned int i=0; i<tasks.size(); i++)
			tasks[i].set_dline(tasks[i].get_wcet());
		normalize_dlines();
	}

	Transaction read_transaction(istream &s)
	{
		int ntasks;
		int period;
		double end_to_end_D;
		s >> ntasks;
		s >> period;
		s >> end_to_end_D;

		Transaction tr(period, end_to_end_D);

		for (int i=0; i<ntasks; i++) {
			double c, d, n;
			s >> c >> d >> n;
			tr.add_task(c, d, n);
		}

		return tr;
	}

	ostream &operator<<(ostream &s, Transaction &tr)
	{
		s << "Name :" << tr.get_name() << endl;
		s << "period :" << tr.get_period()
		  << "  dline : " << tr.get_dline() << endl;
		s << "C \tD \tN" << endl;
		s << "-------------------" << endl;

		for (int i=0; i<tr.get_num_tasks(); i++) {
			s << tr.get_task(i).get_wcet() << "\t"
			  << tr.get_task(i).get_dline() << "\t"
			  << tr.get_task_node(i) << endl;
		}
		return s;
	}

	istream &operator>>(istream &s, Transaction &tr)
	{
		int ntasks;
		int period;
		double end_to_end_D;
		s >> ntasks;
		s >> period;
		s >> end_to_end_D;

		tr.period = period;
		tr.end_to_end_D = end_to_end_D;

		//        Transaction tr(period, end_to_end_D);

		for (int i=0; i<ntasks; i++) {
			double c, d, n;
			s >> c >> d >> n;
			tr.add_task(c, d, n);
		}
		return s;
	}

	double Transaction::get_util(int proc) const
	{
		double c=0;
		for (unsigned int i=0; i<tasks.size(); i++)
			if (allocated[i] == proc) c+=tasks.at(i).get_wcet();

		return c / min(double(get_period()), get_dline());
	}

	vector<double> Transaction::get_utils() const
	{
		int nproc = 0;
		for (unsigned int i=0; i<tasks.size(); i++)
			if ((allocated[i]+1) > nproc) nproc = (allocated[i]+1);

		vector<double> c(nproc);
		for (unsigned int i=0; i<tasks.size(); i++)
			c[allocated[i]] += tasks.at(i).get_wcet();

		for (int j=0; j<nproc; j++)
			c[j] /= min(double(get_period()), get_dline());

		return c;
	}

	void Transaction::clear()
	{
        allocated.clear();
        offsets.clear();
        tasks.clear();


	}


/*        Transaction::operator EDFholistic::Transaction()
	{
			EDFholistic::Transaction htr;

			htr.off = 0;
			htr.T = get_period();
			htr.d = get_dline();
			htr.tasks.clear();
			double temp = 0;
			for (int i=0; i<get_num_tasks(); i++) {
					EDFholistic::Task mytask;
					mytask.C = get_task(i).get_wcet();
					temp += get_task(i).get_dline();
					mytask.d = temp;
					mytask.proc = get_task_node(i);

					htr.tasks.push_back(mytask);
			}

			return htr;

	}*/

}



