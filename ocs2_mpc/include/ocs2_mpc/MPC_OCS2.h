/******************************************************************************
Copyright (c) 2017, Farbod Farshidian. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
******************************************************************************/

#ifndef MPC_OCS2_OCS2_H_
#define MPC_OCS2_OCS2_H_

#include <thread>
#include <mutex>
#include <condition_variable>

#include <ocs2_slq/SLQ_DataCollector.h>
#include <ocs2_ocs2/GSLQ_FW.h>

#include "ocs2_mpc/MPC_SLQ.h"

namespace ocs2 {

/**
 * This an MPC implementation with OCS2 optimal control solver.
 *
 * @tparam STATE_DIM: Dimension of the state space.
 * @tparam INPUT_DIM: Dimension of the control input space.
 * @tparam LOGIC_RULES_T: Logic Rules type.
 */
template <size_t STATE_DIM, size_t INPUT_DIM, class LOGIC_RULES_T>
class MPC_OCS2 : public MPC_SLQ<STATE_DIM, INPUT_DIM, LOGIC_RULES_T>
{
public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	static_assert(std::is_base_of<HybridLogicRules, LOGIC_RULES_T>::value,
			"LOGIC_RULES_T must inherit from HybridLogicRules");

	using Ptr = std::shared_ptr<MPC_OCS2<STATE_DIM, INPUT_DIM, LOGIC_RULES_T>>;

	using slq_data_collector_t = SLQ_DataCollector<STATE_DIM, INPUT_DIM, LOGIC_RULES_T>;
	using gslq_t = GSLQ_FW<STATE_DIM, INPUT_DIM, LOGIC_RULES_T>;

	using BASE = MPC_SLQ<STATE_DIM, INPUT_DIM, LOGIC_RULES_T>;

	using DIMENSIONS = Dimensions<STATE_DIM, INPUT_DIM>;
	using controller_t = typename DIMENSIONS::controller_t;
	using controller_array_t = typename DIMENSIONS::controller_array_t;
	using scalar_t = typename DIMENSIONS::scalar_t;
	using scalar_array_t = typename DIMENSIONS::scalar_array_t;
	using size_array_t = typename DIMENSIONS::size_array_t;
	using state_vector_t = typename DIMENSIONS::state_vector_t;
	using state_vector_array_t = typename DIMENSIONS::state_vector_array_t;
	using state_vector_array2_t = typename DIMENSIONS::state_vector_array2_t;
	using input_vector_t = typename DIMENSIONS::input_vector_t;
	using input_vector_array_t = typename DIMENSIONS::input_vector_array_t;
	using input_vector_array2_t = typename DIMENSIONS::input_vector_array2_t;
	using input_state_matrix_t = typename DIMENSIONS::input_state_matrix_t;
	using input_state_matrix_array_t = typename DIMENSIONS::input_state_matrix_array_t;
	using input_state_matrix_array2_t = typename DIMENSIONS::input_state_matrix_array2_t;
	using dynamic_vector_t = typename DIMENSIONS::dynamic_vector_t;
	using dynamic_vector_array_t = typename DIMENSIONS::dynamic_vector_array_t;

	using mode_sequence_template_t = typename BASE::mode_sequence_template_t;
	using logic_rules_machine_t = typename BASE::logic_rules_machine_t;
	using controlled_system_base_t = typename BASE::controlled_system_base_t;
	using event_handler_t = typename BASE::event_handler_t;
	using derivatives_base_t = typename BASE::derivatives_base_t;
	using constraint_base_t = typename BASE::constraint_base_t;
	using cost_function_base_t = typename BASE::cost_function_base_t;
	using operating_trajectories_base_t = typename BASE::operating_trajectories_base_t;

	/**
	 * Default constructor.
	 */
	MPC_OCS2();

	/**
	 * Constructor
	 *
	 * @param [in] systemDynamicsPtr: The system dynamics which possibly includes some subsystems.
	 * @param [in] systemDerivativesPtr: The system dynamics derivatives for subsystems of the system.
	 * @param [in] systemConstraintsPtr: The system constraint function and its derivatives for subsystems.
	 * @param [in] costFunctionPtr: The cost function (intermediate and terminal costs) and its derivatives for subsystems.
	 * @param [in] operatingTrajectoriesPtr: The operating trajectories of system which will be used for initialization of SLQ.
	 * @param [in] partitioningTimes: This will be used as the initial time partitioning. As the MPC progresses the internal
	 * partitioningTimes will be shifted in time automatically.
	 * @param [in] slqSettings: Structure containing the settings for the SLQ algorithm.
	 * @param [in] mpcSettings: Structure containing the settings for the MPC algorithm.
	 * @param [in] logicRulesPtr: The logic rules used for implementing mixed logical dynamical systems.
	 * @param [in] heuristicsFunctionPtr: Heuristic function used in the infinite time optimal control formulation. If it is not
	 * defined, we will use the terminal cost function defined in costFunctionPtr.
	 */
	MPC_OCS2(const controlled_system_base_t* systemDynamicsPtr,
			const derivatives_base_t* systemDerivativesPtr,
			const constraint_base_t* systemConstraintsPtr,
			const cost_function_base_t* costFunctionPtr,
			const operating_trajectories_base_t* operatingTrajectoriesPtr,
			const scalar_array_t& partitioningTimes,
			const SLQ_Settings& slqSettings = SLQ_Settings(),
			const MPC_Settings& mpcSettings = MPC_Settings(),
			const LOGIC_RULES_T* logicRulesPtr = nullptr,
			const mode_sequence_template_t* modeSequenceTemplatePtr = nullptr,
			const cost_function_base_t* heuristicsFunctionPtr = nullptr);

	/**
	 * Destructor.
	 */
	virtual ~MPC_OCS2();

	/**
	 * Resets the class to its state after construction.
	 */
	void reset() override;

//	/**
//	 * Gets the OCS2 settings structure.
//	 *
//	 * @return OCS2 settings structure
//	 */
//	virtual OCs2_Settings& ocs2Settings();

	/**
	 * The main routine of MPC which runs MPC for the given state and time.
	 *
	 * @param [in] currentTime: The given time.
	 * @param [in] currentState: The given state.
	 */
	virtual bool run(
			const scalar_t& currentTime,
			const state_vector_t& currentState);

protected:
	/**
	 * Rewinds the SLQ-MPC.
	 */
	virtual void rewind();

	void runOCS2();

private:
	typename gslq_t::Ptr gslqPtr_;

	std::thread workerOCS2;

	std::mutex dataCollectorMutex_;

	bool activateOCS2_;
	bool terminateOCS2_;
	std::condition_variable ocs2Synchronization_;

	std::unique_ptr<slq_data_collector_t> slqDataCollectorPtr_;

	scalar_array_t eventTimesOptimized_;
	size_array_t   subsystemsSequenceOptimized_;
};

} // namespace ocs2

#include "implementation/MPC_OCS2.h"

#endif /* MPC_OCS2_OCS2_H_ */
