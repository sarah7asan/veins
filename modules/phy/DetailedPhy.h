/* -*- mode:c++ -*- ********************************************************
 * file:        DetailedPhy.h
 *
 * author:      Marc Loebbers
 *
 * copyright:   (C) 2004 Telecommunication Networks Group (TKN) at
 *              Technische Universitaet Berlin, Germany.
 *
 *              This program is free software; you can redistribute it
 *              and/or modify it under the terms of the GNU General Public
 *              License as published by the Free Software Foundation; either
 *              version 2 of the License, or (at your option) any later
 *              version.
 *              For further information see file COPYING
 *              in the top level directory
 ***************************************************************************
 * part of:     framework implementation developed by tkn
 ***************************************************************************/


#ifndef DETAILED_PHY_LAYER_H
#define DETAILED_PHY_LAYER_H

#include "BasePhyLayer.h"
#include "NicControlType.h"
#include "ActiveChannel.h"

/**
 * @brief a detailed Phy class
 *
 * The DetailedPhy module provides functionality like en- and
 * decapsulation of messages. If you use the standard message formats
 * everythng should work fine. Before a packet is sent some
 * information, e.g. transmission power, can be written to the
 * AirFrame header. If you write your own snrEval, just subclass and
 * redefine the handleUpperMsg function (see description of the
 * function). After receiving a message it can be processed in
 * handleLowerMsgStart. Then it is buffered for the time the
 * transmission would last in reality, and then can be handled
 * again. Again you can redefine the 1. handleLowerMsgStart and
 * 2. handleLowerMsgEnd for your own needs (see description). So, the
 * call of these functions represent the following events: 1. received
 * a message (i.e. transmission startet) 2. message will be handed on
 * to the upper layer (i.e. transmission time is over)
 *
 * @author Marc Loebbers
 * @author Andreas Koepke
 * @author Daniel Willkomm
 * @ingroup snrEval
 * @ingroup basicModules
 */
class DetailedPhy : public BasePhyLayer
{

protected:

    /** brief a parameter that has to be read in from omnetpp.ini*/
    int headerLength;

    /** 
     * @brief power used to transmit messages 
     *
     * The transmission power is initially set to the maximal
     * transmission power possible defined in the ChannelControl
     * module.
     *
     * It can be changed by the user but NEVER to a bigger value than
     * defined in ChannelControl.
     **/
    double transmitterPower;

    /** 
     * @brief carrier frequency used to send messages 
     *
     * The carrier frequency is initially set to the minimum carrier
     * frequency defined in the ChannelControl module.
     *
     * It can be changed by the user but NEVER to a smaller value than
     * defined in ChannelControl.
     **/
    double carrierFrequency;

    /** 
     * @brief signal attenuation threshold 
     *
     * The signal attenuation threshold is initially set to the
     * minimum signal attenuation threshold possible defined in the
     * ChannelControl module.
     *
     * It can be changed by the user but NEVER to a smaller value than
     * defined in ChannelControl.
     **/
   double sensitivity;
    
    /**
     * @brief path loss coefficient
     *
     * The path loss coefficient is initially set to the minimum path
     * loss coefficient possible defined in the ChannelControl module.
     *
     * It can be changed by the user but NEVER to a smaller value than
     * defined in ChannelControl.
     **/
    double alpha;

    /** @brief Speed of light */
    //static const double speedOfLight;

    /** @brief gate id*/
    /*@{*/
    int uppergateOut;
    int uppergateIn;
    int upperControlOut;
    int upperControlIn;
    /*@}*/

    /** @brief Currently active channel, set using radio, updated via BB */
    ActiveChannel channel;
    /** @brief category number given by bb for ActiveChannel */
    int catActiveChannel;


    /** @brief keep bitrate to calculate duration */
    double bitrate;

    /** @brief The kind field of messages
     * 
     * that are used internally by this class have one of these values
     * 
     */
    enum BasicSnrMsgKinds {
        RECEPTION_COMPLETE = 1350101811
    };
    
    /** @brief Last move of this host */
    Move move;
    /** @brief category number given by bb for RSSI */
    int catMove;

public:
    Module_Class_Members( DetailedPhy, BasePhyLayer, 0 );

    /** @brief Initialization of the module and some variables*/
    virtual void initialize(int);

	/** @brief Called by the Blackboard whenever a change occurs we're interested in */
    //virtual void receiveBBItem(int category, const BBItem *details, int scopeModuleId);
    
protected:
    /**
     * @name Handle Messages
     * @brief Functions to redefine by the programmer
     *
     * These are the functions provided to add own functionality to your
     * modules. These functions are called whenever a blackboard
     * message, a self message or a data message from the upper or lower
     * layer arrives respectively.
     *
     **/
    /*@{*/

    /** @brief Calculate Snr Information before buffering
     *
     * This function is called right after a message is received,
     * i.e. right before it is buffered for 'transmission time'.
     *
     * Here you should decide whether the message is "really" received
     * or whether it's receive power is so low that it is just treated
     * as noise.
     *
     * If the energy of the message is high enough to really receive
     * it you should create an snr list (@ref SnrList) to be able to
     * store sn(i)r information for that message. Every time a new
     * message arrives you can add a new snr value together with a
     * timestamp to that list. Make sure to store a pointer to the
     * mesage together with the snr information to be able to retrieve
     * it later.
     *
     * In this function also an initial SNR value can be calculated
     * for this message.
     *
     * Please take a look at SnrEval to see a "real" example.
     *
     * @sa SnrList, SnrEval
     **/
    virtual void handleLowerMsgStart(cMessage*);

    /**
     * @brief Calculate SnrInfo after buffering and add the PhySnrList
     * to the message
     *
     * Redefine this function if you want to process messages from the
     * channel before they are forwarded to upper layers
     */
    virtual void handleLowerMsgEnd(cMessage*);

    /*@}*/

    /** @brief Encapsulates a MAC packet into an AirFrame*/
    virtual AirFrame* encapsMsg(cMessage *msg);

	/** @brief Calculate duration of this message */
    virtual double calcDuration(cMessage* m) {
        return static_cast<double>(m->length()) / bitrate;
    }
};

#endif
