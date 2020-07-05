/*
 * Copyright (C) 2017 Rahul Jadhav <nyrahul@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU
 * General Public License v2. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     airline
 * @{
 *
 * @file
 * @brief       Propagation Loss Models from NS3 for Whitefield
 *
 * @author      Rahul Jadhav <nyrahul@gmail.com>
 *
 * @}
 */

#ifndef _PROPAGATIONMODEL_H_
#define _PROPAGATIONMODEL_H_

#include <ns3/ptr.h>
#include <ns3/propagation-loss-model.h>
#include <ns3/propagation-delay-model.h>

using namespace ns3;
using namespace std;

Ptr<PropagationLossModel> getLossModel(string loss_model,
                                       string loss_model_param);

Ptr<PropagationDelayModel> getDelayModel(string del_model,
                                         string del_model_param);

#endif //	_PROPAGATIONMODEL_H_
