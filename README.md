# LIP Key Press Behavior

Implementation of [LAST INPUT PRIORITY](https://www.hitboxarcade.com/blogs/support/what-is-socd) key press for ZMK.

## Installation

Include this project on your ZMK's west manifest in `config/west.yml`:

```yaml
manifest:
  remotes:
    # START +++
    - name: badjeff
      url-base: https://github.com/badjeff
    # END +++
  projects:
    # START +++
    - name: zmk-behavior-key-press-lip
      remote: badjeff
      revision: main
    # END +++
  self:
    path: config
```

Now, update your `shield.keymap` adding the behaviors.

```keymap
/ {
        behaviors {
              kpad: key_kip_a_d {
                      compatible = "zmk,behavior-key-press-lip";
                      #binding-cells = <1>;
                      keycodes = <A D>;
              };
              kpws: key_kip_w_s {
                      compatible = "zmk,behavior-key-press-lip";
                      #binding-cells = <1>;
                      keycodes = <W S>;
              };
        };

        keymap {
                compatible = "zmk,keymap";
                wasd_layer {
                        bindings = <
                              ...
                              ...       &kpws W   ...       ...
                              &kpad A   &kpws S   &kpad D   ...
                              ...
                        >;
                };
       };

};
```
