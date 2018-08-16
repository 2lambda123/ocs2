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

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <gtest/gtest.h>

#include <ocs2_slq/SLQ.h>
#include <ocs2_slq/SLQ_MP.h>
#include <ocs2_slq/test/EXP0.h>

#include "ocs2_ocs2/OCS2Projected.h"

using namespace ocs2;

enum
{
	STATE_DIM = 2,
	INPUT_DIM = 1
};

TEST(exp0_ocs2_test, exp0_ocs2_test)
{
	// system dynamics
	EXP0_System systemDynamics;

	// system derivatives
	EXP0_SystemDerivative systemDerivative;

	// system constraints
	EXP0_SystemConstraint systemConstraint;

	// system cost functions
	EXP0_CostFunction systemCostFunction;

	// system operatingTrajectories
	Eigen::Matrix<double,STATE_DIM,1> stateOperatingPoint = Eigen::Matrix<double,STATE_DIM,1>::Zero();
	Eigen::Matrix<double,INPUT_DIM,1> inputOperatingPoint = Eigen::Matrix<double,INPUT_DIM,1>::Zero();
	EXP0_SystemOperatingTrajectories operatingTrajectories(stateOperatingPoint, inputOperatingPoint);


	/******************************************************************************************************/
	/******************************************************************************************************/
	/******************************************************************************************************/
	SLQ_Settings slqSettings;
	slqSettings.displayInfo_ = false;
	slqSettings.displayShortSummary_ = false;
	slqSettings.displayGradientDescent_ = true;
	slqSettings.maxNumIterationsSLQ_ = 50;
	slqSettings.minLearningRateGSLQP_ = 0.01;
	slqSettings.absTolODE_ = 1e-10;
	slqSettings.relTolODE_ = 1e-7;
	slqSettings.maxNumStepsPerSecond_ = 50000;
	slqSettings.nThreads_ = 2;
	slqSettings.useMultiThreading_ = false;  // no multi-thread
	slqSettings.maxNumIterationsSLQ_ = 50;
	slqSettings.maxIterationGradientDescent_ = 10;
	slqSettings.warmStartGSLQ_ = false;
	slqSettings.minLearningRateNLP_ = 0.01;
	slqSettings.useAscendingLineSearchNLP_ = false;

	// switching times
	std::vector<double> initEventTimes {1.0};
	EXP0_LogicRules logicRules(initEventTimes);

	double startTime = 0.0;
	double finalTime = 2.0;

	// partitioning times
	std::vector<double> partitioningTimes;
	partitioningTimes.push_back(startTime);
	partitioningTimes.push_back(1.0);
	partitioningTimes.push_back(finalTime);

	Eigen::Vector2d initState(0.0, 2.0);

	/******************************************************************************************************/
	/******************************************************************************************************/
	/******************************************************************************************************/
	// GSLQ - single core version
	OCS2Projected<STATE_DIM, INPUT_DIM, EXP0_LogicRules> ocs2(
			&systemDynamics, &systemDerivative,
			&systemConstraint, &systemCostFunction,
			&operatingTrajectories, slqSettings, &logicRules);

	// run ocs2 using LQ
	ocs2.slqSettings().useLQForDerivatives_ = true;
	ocs2.run(startTime, initState, finalTime, partitioningTimes, initEventTimes);
	// optimized event times
	std::vector<double> optEventTimes_LQ(initEventTimes.size());
	ocs2.getEventTimes(optEventTimes_LQ);
	// optimized cost
	double cost_LQ;
	ocs2.getCostFunction(cost_LQ);

	// run ocs2 using BVP
	ocs2.slqSettings().useLQForDerivatives_ = false;
	ocs2.run(startTime, initState, finalTime, partitioningTimes, initEventTimes);
	// optimized event times
	std::vector<double> optEventTimes_BVP(initEventTimes.size());
	ocs2.getEventTimes(optEventTimes_BVP);
	// optimized cost
	double cost_BVP;
	ocs2.getCostFunction(cost_BVP);


	/******************************************************************************************************/
	/******************************************************************************************************/
	/******************************************************************************************************/
	std::cerr << "### Initial event times are: [" << initEventTimes[0] << "]\n";

	std::cerr << "### Optimum cost LQ method: " << cost_LQ << std::endl;

	std::cerr << "### Optimum event times LQ method: [" << optEventTimes_LQ[0] << "]\n";

	std::cerr << "### Optimum cost BVP method: " << cost_BVP << std::endl;

	std::cerr << "### Optimum event times BVP method: [" << optEventTimes_BVP[0] << "]\n";

	const double optimumCost = 9.766;
	const std::vector<double> optimumEventTimes {0.1897};

	ASSERT_NEAR(cost_LQ, optimumCost, 10*slqSettings.minRelCostGSLQP_) <<
			"MESSAGE: OCS2 failed in the EXP1 using LQ approach for calculating derivatives!";

	ASSERT_NEAR(cost_BVP, optimumCost, 10*slqSettings.minRelCostGSLQP_) <<
			"MESSAGE: OCS2 failed in the EXP1 using BVP approach for calculating derivatives!";
}


int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

