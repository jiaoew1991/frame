import photohash

for src in ['1', '2', '3', '4', '5']:
    for dst in ['1', '2', '3', '4', '5']:
        distance = photohash.distance('../resources/{}.png'.format(src), '../resources/{}.png'.format(dst))
        print 'src {} -> dst {}: distance is {}'.format(src, dst, distance)
