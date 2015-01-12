#ifndef SIGNALBLOCKERINTERFACE_H
#define SIGNALBLOCKERINTERFACE_H

class SignalBlockerInterface
{
public:
	SignalBlockerInterface() { m_blockCount = 0; }
	
	void disableSignal () { m_blockCount++; }
	void enableSignal () { m_blockCount--; }
	bool isSignalEnabled () { return (m_blockCount == 0); }
	
private:
	int m_blockCount;
};

#endif // SIGNALBLOCKERINTERFACE_H
