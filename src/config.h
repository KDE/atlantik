#ifndef ATLANTIK_CONFIG_H
#define ATLANTIK_CONFIG_H

#define	MAXPLAYERS	6

struct AtlantikConfig
{
	// Personalization options
	QString playerName;

	// Board options
	bool indicateUnowned;
	bool highliteUnowned;
	bool grayOutMortgaged;
	bool animateToken;
	bool quartzEffects;
};

#endif
