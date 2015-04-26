{
  "targets": [{
    "target_name": "offgrid",
    "sources": [
      "offgrid.cc",
      "offgrid-lights.cc",
      "elinux-tcl/tclled.cc"
    ],
    "defines": ['SPIFILE="/dev/spidev0.0"'],
    "include_dirs": [
      "/opt/vc/include",
      "/opt/vc/include/interface/vcos/pthreads",
      "/opt/vc/include/interface/vmcs_host/linux"
    ],
    "libraries": [
      "-L/opt/vc/lib",
      "-lmmal",
      "-lvcos",
      "-lbcm_host",
      "-lGLESv2",
      "-lEGL",
      "-lm"
    ]
  }]
}
