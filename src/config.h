#ifndef __KMONOP_CONFIG_H__
#define __KMONOP_CONFIG_H__

#define	MAXPLAYERS	6
#define VERSION		"pre-0.2.0"

struct KMonopConfig
{
	// Personalization options
	QString playerName;

	// Board options
	bool indicateUnowned;
	bool grayOutMortgaged;
	bool animateToken;
};

#endif
