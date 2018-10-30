#ifndef NUMBEREDIT_H
#define NUMBEREDIT_H

#include <QLineEdit>
#include <cmath>
#include <limits>


/**
 * @brief A QLineEdit specialized for numerical values.
 *
 * This class should be used for all kind of user input where numbers are expected. It can take care of
 * a value range, allows only valid numbers and can set the precision of the numbers to a specified value.
 * Connect the signal valueChanged to react to a changed value.
 */
class NumberEdit : public QLineEdit
{
	Q_OBJECT
	
public:
	enum OutputFormat {Standard, Scientific};
	
	NumberEdit(OutputFormat format = Standard, int automaticPrecision = 8,
			   double minimum = std::numeric_limits<double>::lowest(), double maximum = std::numeric_limits<double>::max());
	
	/**
	 * @brief Sets the value of the edit.
	 *
	 * The given value is fit into the range. If the value is too high or low it will be set to minimum 
	 * respectively maximum. If chopPrecision is set or the edit is intended for integer (precision = 0)
	 * the value is mathematicaly rounded.
	 * @param newValue The new value of the edit.
	 * @param chopPrecision If the value will be rounded to the set precision or not.
	 */
	void setValue (double newValue, bool chopPrecision = true);
	
	/**
	 * @brief returns the numerical value of the edit.
	 *
	 * In some rare cases (e.g. in the Delegate classes, where the setModelData is called before the
	 * enditingFinished signal is emited) it is important to process the String inside the edit before
	 * returning the numerical value.
	 * @param processValueFirst Wheter to processes the string immediately before returning value.
	 * @return The numerical value of the edit.
	 */
	double getValue (bool processValueFirst = false);
	
	/**
	 * @brief Sets the minimal value that will be accepted.
	 *
	 * This will also check the current value and update it if necessary.
	 * @param newMinimum The new minimum.
	 */
	void setMinimum (double newMinimum);
	
	/**
	 * @brief Sets the maximal value that will be accepted.
	 *
	 * This will also check the current value and update it if necessary.
	 * @param newMaximum The new maximum.
	 */
	void setMaximum (double newMaximum);
	
	/**
	 * @brief Sets both the minimal and maximal value that will be accepted.
	 *
	 * This will also check the current value and update it if necessary.
	 * @param newMinimum The new minimum.
	 * @param newMaximum The new maximum.
	 */	 
	void setRange (double newMinimum, double newMaximum);
	
	/**
	 * @brief Sets how many digits after decimal point will be stored if chopPrecision is run.
	 *
	 * Set this property to zero if the edit is intended for integer. For user input (exept for precision = 0)
	 * this value is not used because chopPrecision is deactivated.
	 * @param newPrecision The number of digits after decimal point.
	 */
	void setAutomaticPrecision (int newPrecision);
	
	/**
	 * @brief Whether the value is formatted in scientific or standard representation.
	 *
	 * @param newFormat The used format.
	 */
	void setFormat (OutputFormat newFormat);
	
private:
	/**
	 * @brief Formats the value and creates a string representation to fill the edit.
	 *
	 * Trailing zeros will be choped.
	 */
	void showValue ();
	
	double m_value;  /**< The numerical value of the edit */
	double m_maximum;  /**< The highest accepted value */
	double m_minimum;  /**< The lowest accepted value */
	int m_automaticPrecision;  /**< The number of digits after decimal point */
	OutputFormat m_format;  /**< The format of the number in the edit */
	
private slots:
	/**
	 * @brief Tries to transform user input into a number and if successful sets this value.
	 */
	void onEditingFinished ();
	
signals:
	/**
	 * @brief Is emited whenever the value really has changed.
	 */
	void valueChanged (double newValue);
};

#endif // NUMBEREDIT_H
