BEGIN {
	simTime = 600;
	convTime = 81;
}

{
	time = $1;
	imsi = $4;
	rx_bytes = $10;

	if ( time < convTime ) {
		total_bytes_1[imsi] += rx_bytes;
	}
	else if ( time >= convTime ) {
		total_bytes_2[imsi] += rx_bytes;
	}

	total_bytes[imsi] += rx_bytes;

}

END {
	printf ("First part -------------\n\n");
	for ( imsi in total_bytes_1 ) {
		if ( total_bytes_1[imsi] > 0 ) {
			printf ( "%d %d %d %f\n", imsi, total_bytes_1[imsi], convTime, total_bytes_1[imsi]*8/convTime );
		} else {
			printf ( "%d %d %d %d\n", imsi, total_bytes_1[imsi], convTime, 0 );
		}
	}
	printf ("\n\nSecond part -------------\n\n");

	for ( imsi in total_bytes_2 ) {
		if ( total_bytes_2[imsi] > 0 ) {
			tdelta = simTime - convTime;
			printf ( "%d %d %d %f\n", imsi, total_bytes_2[imsi], tdelta, total_bytes_2[imsi]*8/tdelta);
		} else {
			printf ( "%d %d %d %d\n", imsi, total_bytes_2[imsi], tdelta, 0 );
		}
	}

	printf ("\n\nTotal -------------\n\n");
	for ( imsi in total_bytes ) {
		if ( total_bytes[imsi] > 0 ) {
			printf ( "%d %d %d %f\n", imsi, total_bytes[imsi], simTime, total_bytes[imsi]*8/simTime);
		} else {
			printf ( "%d %d %d %d\n", imsi, total_bytes[imsi], simTime, 0 );
		}
	}
}
