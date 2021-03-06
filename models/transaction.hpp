#ifndef __TRANSACTION_H__
#define __TRANSACTION_H__

#include <vector>
#include <string>
#include <iostream>

#include <models/task.hpp>
#include <models/taskset.hpp>

// forward declaration
namespace EDFholistic {
    class Transaction;
}

namespace Scan {
	/**
	   This class models a transaction, i.e. a set of tasks to be
	   executed in a sequence on different computational nodes.

	   Right now, a transaction is identified by a period, a
	   end-to-end deadline and a list of tasks.

	   Tasks are identified by wcet, relative deadline.

	   Each task is statically allocated to a node (identified by a
	   simple numerical index)
	*/
	class Transaction {
		std::string name;
		int id;
		int period;
		double end_to_end_D;
		TaskSet tasks;
		std::vector<int> allocated;
		std::vector<double> offsets;

		friend std::istream &operator>>(std::istream &s, Transaction &tr);

	public:
		Transaction();

		void setid(int i);
		int getid();

		/**
		   Constructor of the transaction. To insert tasks, you have
		   to call the add_task() method for all tasks in the right
		   order.

		   @param p period of the transaction
		   @param d deadline (end-to-end) of the transaction
		*/
		Transaction(int p, double d);

		/**
		   returns the task name
		*/
		std::string get_name();


		/**
		   Add a new task at the end of the transaction. The tasks is
		   allocated on the specified node. The task period is
		   ignored, because all tasks are assigned the same period of
		   the transaction.

		   @param t task to be added. The task object is copied in the
		   transaction.

		   @param node index of the node on which this task is
		   allocated
		*/
		void add_task(const Task &t, int node);

		/**
		   Add a new task at the end of the transaction. The tasks is
		   allocated on the specified node.

		   @param c worst-case computation time

		   @param d relative deadline (relative to the previous task deadline, or
		   to the beginning of the transaction for the first task)

		   @param node index of the node on which this task is
		   allocated
		*/
		void add_task(double c, double d, int node);

		/**
		   Returns the transaction period
		*/
		int get_period() const;

		/**
		   Sets a new period
		*/
		void set_period(const int p);

		/**
		   Returns the transaction end to end deadline
		*/
		double get_dline() const;

		/**
		   Sets a new end to end deadline
		*/
		void set_dline(const double dl);

		/**
		   Returns the i-th task. The task could be modified,
		   so this accessor is not declared as constant.
		*/
		Task &get_task(unsigned int i);

		/**
		   Returns the i-th task. The task is copied,
		   so this accessor is declared constant.
		 */
		const Task get_const_task(unsigned int i) const;

		/**
		   Returns the node index on which the i-th task is allocated
		*/
		int get_task_node(unsigned int i) const;

		/**
   Returns the offset of the i-th task
*/
double get_task_offset(unsigned int i);

		/**
		   Returns the number of tasks in the transaction
		*/
		int get_num_tasks() const;

		/**
		   This function normalizes task relative deadlines so that
		   their sum equals the end-to-end deadline.
		*/
		void normalize_dlines();

		/**
		   This function sets deadline proportional to computation
		   times, so that their sum equal the end-to-end deadline.
		*/
		void proportional_dlines();

		/**
		   Returns the utilizazion on processor proc. This is the sum
		   of the computation times of all the tasks allocated on that
		   processor divided by the minimum between period and end-to-end
		   deadline.
		*/
		double get_util(int proc) const;

		/**
		   Returns the utilizations on all processors. This is the sum
		   of the computation times of all the tasks allocated on that
		   processor divided by the minimum between period and end-to-end
		   deadline.
		*/
		std::vector<double> get_utils() const;

		/**
		   Conversion operator
		*/
		//operator EDFholistic::Transaction();
	};

	/**
	   Function that creates a transaction, reading from an istream.
	   The stream must contain the following numerical data (separated
	   by spaces or new lines):

	   - number of tasks
	   - period
	   - deadline

	   For each task:

	   - computation time
	   - relative deadline
	   - node index
	*/
	Transaction read_transaction(std::istream &s);

	/**
	   Like read_transactions, but using the >> stream operator
	*/
	std::istream &operator>>(std::istream &s, Transaction &tr);

	/**
	   Output the stransaction on the specified stream, with the same
	   order as specified in read_transaction
	*/
	std::ostream &operator<<(std::ostream &s, Transaction &tr);
}


#endif
