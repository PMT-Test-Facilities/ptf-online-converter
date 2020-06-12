/*
 * Copyright 2006 Phidgets Inc.  All rights reserved.
 */

package com.phidgets.event;

import com.phidgets.Phidget;

/**
 * This class represents the data for a StepperVelocityChangeEvent.
 * 
 * @author Phidgets Inc.
 */
public class StepperVelocityChangeEvent
{
	Phidget source;
	int index;
	double value;

	/**
	 * Class constructor. This is called internally by the phidget library when creating this event.
	 * 
	 * @param source the Phidget object from which this event originated
	 */
	public StepperVelocityChangeEvent(Phidget source, int index, double value) {
		this.source = source;
		this.index = index;
		this.value = value;
	}

	/**
	 * Returns the source Phidget of this event. This is a reference to the Phidget object from which this
	 * event was called. This object can be cast into a specific type of Phidget object to call specific
	 * device calls on it.
	 * 
	 * @return the event caller
	 */
	public Phidget getSource() {
		return source;
	}

	/**
	 * Returns the index of the Stepper.
	 * 
	 * @return the index of the Stepper
	 */
	public int getIndex() {
		return index;
	}

	/**
	 * Return the velocity of the Stepper. This is reported back from the Stepper controller as the Stepper changes speed.
	 * 
	 * @return the Stepper velocity
	 */
	public double getValue() {
		return value;
	}

	/**
	 * Returns a string containing information about the event.
	 * 
	 * @return an informative event string
	 */
	public String toString() {
		return source.toString() + " Stepper velocity " + index + " changed to "
		  + value;
	}
}
