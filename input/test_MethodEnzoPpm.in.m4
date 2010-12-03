Domain {
   extent = [0.0, 0.3, 0.0, 0.3];
}
Stopping {
  time = 2.5;
  cycle = 100;
}
Mesh {
   root_size  = [N, N];  # N x N
   patch_size = [N, N];  # N <= mp <= N
   block_size = [N, N];  # N <= mb <= N
}
Field {
   ghosts = [3,3];
   fields = ["density","velocity_x","velocity_y","total_energy","internal_energy"];
}
Method {
   sequence = ["ppm"];
}
Physics {
   dimensions = 2;
   gamma = 1.4;
   cosmology = false;
#    cosmology {
#        comoving_box_size = 64.0;
#        initial_redshift = 20.0;
#        hubble_constant_now = 0.5;
#        omega_matter_now = 1.0;
#        omega_lambda_now = 0.0;
#        max_expansion_rate = 0.01;
#   }
}

     
