#ifndef __KMONOP_CONFIG_H__
#define __KMONOP_CONFIG_H__

#define	MAXPLAYERS	6
#define VERSION		"0.1.0"

struct KMonopConfig
{
	// Personalization options
	QString playerName;

	// Board options
	bool indicateUnowned;
	bool animateToken;
};

#endif
