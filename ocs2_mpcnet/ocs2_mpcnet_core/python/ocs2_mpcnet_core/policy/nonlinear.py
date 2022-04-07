###############################################################################
# Copyright (c) 2022, Farbod Farshidian. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
#  * Redistributions of source code must retain the above copyright notice, this
#   list of conditions and the following disclaimer.
#
#  * Redistributions in binary form must reproduce the above copyright notice,
#   this list of conditions and the following disclaimer in the documentation
#   and/or other materials provided with the distribution.
#
#  * Neither the name of the copyright holder nor the names of its
#   contributors may be used to endorse or promote products derived from
#   this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
###############################################################################

"""Nonlinear policy.

Provides a class that implements a nonlinear policy.
"""

import torch


class NonlinearPolicy(torch.nn.Module):
    """Nonlinear policy.

    Class for a simple nonlinear neural network policy, where the hidden layer dimension is the mean of the input and
    output dimensions.

    Attributes:
        name: A string with the name of the policy.
        observation_dimension: An integer defining the observation (i.e. input) dimension of the policy.
        hidden_dimension: An integer defining the dimension of the hidden layer.
        action_dimension: An integer defining the action (i.e. output) dimension of the policy.
        linear1: The first linear neural network layer.
        activation: The activation to get the hidden layer.
        linear2: The second linear neural network layer.
    """

    def __init__(self, observation_dimension: int, action_dimension: int) -> None:
        """Initializes the NonlinearPolicy class.

        Initializes the NonlinearPolicy class by setting fixed and variable attributes.

        Args:
            observation_dimension: An integer defining the observation dimension.
            action_dimension: An integer defining the action dimension.
        """
        super().__init__()
        self.name = "NonlinearPolicy"
        self.observation_dimension = observation_dimension
        self.hidden_dimension = int((observation_dimension + action_dimension) / 2)
        self.action_dimension = action_dimension
        self.linear1 = torch.nn.Linear(self.observation_dimension, self.hidden_dimension)
        self.activation = torch.nn.Tanh()
        self.linear2 = torch.nn.Linear(self.hidden_dimension, self.action_dimension)

    def forward(self, observation: torch.Tensor) -> torch.Tensor:
        """Forward method.

        Defines the computation performed at every call. Computes the output tensors from the input tensors.

        Args:
            observation: A (B,O) tensor with the observations.

        Returns:
            action: A (B,A) tensor with the predicted actions.
        """
        return self.linear2(self.activation(self.linear1(observation)))
