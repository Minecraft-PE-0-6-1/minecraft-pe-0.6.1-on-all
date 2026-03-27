#pragma once

//package util;

class SmoothFloat
{
    float targetValue;
    float remainingValue;
    float lastAmount;

public:
	SmoothFloat()
	:	targetValue(0),
		remainingValue(0),
		lastAmount(0)
	{}

    float getNewDeltaValue(float deltaValue, float accelerationAmount) {
        targetValue += deltaValue;

        deltaValue = (targetValue - remainingValue) * accelerationAmount;
        lastAmount = lastAmount + (deltaValue - lastAmount) * .5f;
        if ((deltaValue > 0 && deltaValue > lastAmount) || (deltaValue < 0 && deltaValue < lastAmount)) {
            deltaValue = lastAmount;
        }
        remainingValue += deltaValue;

        return deltaValue;
    }

    float getTargetValue() {
        return targetValue;
    }
};

