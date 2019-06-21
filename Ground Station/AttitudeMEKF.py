import numpy as np
import pyquaternion as pq

class AttitudeMEKF:
	def __init__():
		self.lastUpdate=None
		self.q_ref=pq()
		self.w_ref=np.array(3,1)
		self.aCovar=np.zeros((3,3))

		self.state=np.zeros(9)
		self.stateCovar=np.zeros((9,9))

	def position(self):
		return self.state[0:2]
	def velocity(self):
		return self.state[3:5]
	def acceleration(self):
		return self.state[6:8]
	def earthToVehicleRot(self):
		return self.q_ref
	def vehicleToEarthRot(self):
		return self.q_ref.inverse

	def stateTranferMat(self,dt):
		return np.block([
			[np.eye(3), dt*np.eye(3), 0.5*dt*dt*np.eye(3)],
			[np.zeros((3,3)), np.eye(3), dt*np.eye(3)],
			[np.zeros((3,6)), np.eye(3)]
			])

	def predictStep(self,dt):
		posMat = self.stateTranferMat(dt)
		predictState = posMat @ self.state
		predictCovar = posMat @ self.stateCovar @ posMat.T # + Q?

		predictRef = pq(axis=w_ref, angle=dt*norm(w_ref)) * state.q_ref;
		rotMat = -crossMat(w_ref)
		predictPa = rotMat @ self.aCovar @ rotMat.T # + Q?
		return predictState, predictCovar, predictRef, predictPa

	def updateAltimeter(self,time,alt):
		dt = time - self.lastUpdate
		self.lastUpdate = time

		pState, pCovar, pRef, pPa = self.predictStep(dt)

		resid = alt-pState[0]
		rvar = pCovar[0,0] # + Altimeter variance
		gain = pCovar[0,:]/rvar

		self.state = pState + gain * resid
		self.stateCovar = (np.eye(9)-np.hstack(gain,np.zeros((9,8)))) @ pCovar
		# No w_ref update since rotation is not involved in predicting altitude
		self.q_ref = 

	def updateAccelGyro(self,time,accelz,gyroz):
		dt = time - self.lastUpdate
		self.lastUpdate = time

		pState, pCovar = self.predictStep(dt)


def crossMat(vector):
	return np.array([
		[0, -vector[2], vector[1]],
		[vector[2], 0, -vector[0]],
		[-vector[1], vector[0], 0]
		])