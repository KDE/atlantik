#ifndef __KMONOP_CONFIG_H__
#define __KMONOP_CONFIG_H__

#define	MAXPLAYERS	6
#define VERSION		"pre-0.0.2"

struct KMonopConfig
{
	// Personalization options
	QString playerName;

	// Board options
	bool indicateUnowned;
	bool animateToken;
};

#endif
