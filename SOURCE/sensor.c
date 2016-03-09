//
// Zealtek D2 project
// Sensor I2C Library
// This library is used for both Sensor 0 and Sensor 1 interface.
// Check out sensor0.c and sensor1.c for proper usages.
//
// Zealtek internal usage only.
// All files in this project cannot be released to customers.
//
// May 21, 2012.


#ifdef I2C_INTERFACE

tI2CWorkItem I2CWorkItem;

unsigned char fnI2cWriteRegisterI2c()
{
	unsigned char ucI2cFailCounter;

	for (ucI2cFailCounter = 0; ucI2cFailCounter < 3; ucI2cFailCounter++)
	{
		fnI2cStartI2c();
		{
			if (fnI2cWriteDataI2c(I2CWorkItem.I2CSlaveAddress&0xFE))
			{
				fnI2cStopI2c();
				continue;
			}
		}
		if (I2CWorkItem.I2CConfiguration&I2C_REGISTER_ADDRESS_WORD)
		{
			if (fnI2cWriteDataI2c(GETWORDMSB(I2CWorkItem.I2CRegAddress)))
			{
				fnI2cStopI2c();
				continue;
			}
		}
		{
			if (fnI2cWriteDataI2c(GETWORDLSB(I2CWorkItem.I2CRegAddress)))
			{
				fnI2cStopI2c();
				continue;
			}
		}
		if (I2CWorkItem.I2CConfiguration&I2C_REGISTER_VALUE_WORD)
		{
			if (fnI2cWriteDataI2c(GETWORDMSB(I2CWorkItem.I2CRegValue)))
			{
				fnI2cStopI2c();
				continue;
			}
		}
		{
			if (fnI2cWriteDataI2c(GETWORDLSB(I2CWorkItem.I2CRegValue)))
			{
				fnI2cStopI2c();
				continue;
			}
		}
		fnI2cStopI2c();
		return SUCCESS;
	}
	return FAILURE;
}

unsigned char fnI2cReadRegisterI2c()
{
	unsigned char ucI2cFailCounter;

	for (ucI2cFailCounter = 0; ucI2cFailCounter < 3; ucI2cFailCounter++)
	{
		fnI2cStartI2c();
		{
			if (fnI2cWriteDataI2c(I2CWorkItem.I2CSlaveAddress&0xFE))
			{
				fnI2cStopI2c();
				continue;
			}
		}
		if (I2CWorkItem.I2CConfiguration&I2C_REGISTER_ADDRESS_WORD)
		{
			if (fnI2cWriteDataI2c(GETWORDMSB(I2CWorkItem.I2CRegAddress)))
			{
				fnI2cStopI2c();
				continue;
			}
		}
		{
			if (fnI2cWriteDataI2c(GETWORDLSB(I2CWorkItem.I2CRegAddress)))
			{
				fnI2cStopI2c();
				continue;
			}
		}
		if (I2CWorkItem.I2CConfiguration&I2C_RESTART_READ)
		{
			fnI2cReStartI2c();
		}
		else
		{
			fnI2cStopI2c();
			fnI2cStartI2c();
		}
		{
			if (fnI2cWriteDataI2c(I2CWorkItem.I2CSlaveAddress|0x01))
			{
				fnI2cStopI2c();
				continue;
			}
		}
		if (I2CWorkItem.I2CConfiguration&I2C_REGISTER_VALUE_WORD)
		{
			SETWORDMSB(I2CWorkItem.I2CRegValue, fnI2cReadDataI2c());
		}
		{
			SETWORDLSB(I2CWorkItem.I2CRegValue, fnI2cReadDataI2c());
		}
		fnI2cStopI2c();
		return SUCCESS;
	}
	return FAILURE;
}

unsigned char fnI2cWriteI2c(
	unsigned char *i2cStruct)
{
	{
		SETWORDLSB(I2CWorkItem.I2CRegAddress, *i2cStruct);
		i2cStruct++;
	}
	if (I2CWorkItem.I2CConfiguration&I2C_REGISTER_ADDRESS_WORD)
	{
		SETWORDMSB(I2CWorkItem.I2CRegAddress, *i2cStruct);
		i2cStruct++;
	}
	{
		SETWORDLSB(I2CWorkItem.I2CRegValue, *i2cStruct);
		i2cStruct++;
	}
	if (I2CWorkItem.I2CConfiguration&I2C_REGISTER_VALUE_WORD)
	{
		SETWORDMSB(I2CWorkItem.I2CRegValue, *i2cStruct);
		i2cStruct++;
	}
	return fnI2cWriteRegisterI2c();
}

unsigned char fnI2cReadI2c(
	unsigned char *i2cStruct)
{
	{
		SETWORDLSB(I2CWorkItem.I2CRegAddress, *i2cStruct);
		i2cStruct++;
	}
	if (I2CWorkItem.I2CConfiguration&I2C_REGISTER_ADDRESS_WORD)
	{
		SETWORDMSB(I2CWorkItem.I2CRegAddress, *i2cStruct);
		i2cStruct++;
	}

	if (fnI2cReadRegisterI2c() == FAILURE)
		return FAILURE;

	{
		*i2cStruct = GETWORDLSB(I2CWorkItem.I2CRegValue);
		i2cStruct++;
	}
	if (I2CWorkItem.I2CConfiguration&I2C_REGISTER_VALUE_WORD)
	{
		*i2cStruct = GETWORDMSB(I2CWorkItem.I2CRegValue);
		i2cStruct++;
	}
	return SUCCESS;
}

#ifdef SUPPORT_D2_OPERATION

unsigned char fnI2cWriteTable(
	tI2CRegister code *i2cTable,
	unsigned short usLength)
{
	unsigned short idx;
	tI2CRegister I2CRegister;

	for (idx = 0; idx < usLength; idx ++)
	{
		I2CRegister   = i2cTable[idx];
	#ifdef DBG
		fnUartTxChar('I');
		fnUartTxChar('2');
		fnUartTxChar('C');
		fnUartTxChar(':');
		#if SENSOR_REGISTER_ADDRESS == 2
		fnUartTx8bit(GETWORDMSB(I2CRegister.I2CRegAddress));
		fnUartTx8bit(GETWORDLSB(I2CRegister.I2CRegAddress));
		#else  // SENSOR_REGISTER_ADDRESS
		fnUartTx8bit(I2CRegister.I2CRegAddress);
		#endif // SENSOR_REGISTER_ADDRESS
		fnUartTxChar('=');
		#if SENSOR_REGISTER_VALUE == 2
		fnUartTx8bit(GETWORDMSB(I2CRegister.I2CRegValue));
		fnUartTx8bit(GETWORDLSB(I2CRegister.I2CRegValue));
		#else  // SENSOR_REGISTER_VALUE
		fnUartTx8bit(I2CRegister.I2CRegValue);
		#endif // SENSOR_REGISTER_VALUE
		fnUartTxChar('\n');
		fnUartTxChar('\r');
	#endif // DBG
	#if SENSOR_REGISTER_ADDRESS == 2
		I2CWorkItem.I2CRegAddress = I2CRegister.I2CRegAddress;
	#else  // SENSOR_REGISTER_ADDRESS
		SETWORDLSB(I2CWorkItem.I2CRegAddress, I2CRegister.I2CRegAddress);
	#endif // SENSOR_REGISTER_ADDRESS
	#if SENSOR_REGISTER_VALUE == 2
		I2CWorkItem.I2CRegValue = I2CRegister.I2CRegValue;
	#else  // SENSOR_REGISTER_VALUE
		SETWORDLSB(I2CWorkItem.I2CRegValue, I2CRegister.I2CRegValue);
	#endif // SENSOR_REGISTER_VALUE
		if (fnI2cWriteRegisterI2c() == FAILURE)
		{
	#ifdef SUPPORT_I2C_ERROR_COUNT
			I2CErrorCount ++;
	#else  // SUPPORT_I2C_ERROR_COUNT
			return FAILURE;
	#endif // SUPPORT_I2C_ERROR_COUNT
		}
	}
	return SUCCESS;
}

#endif // SUPPORT_D2_OPERATION

#endif // I2C_INTERFACE
