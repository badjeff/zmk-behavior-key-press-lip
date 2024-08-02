# LIP Key Press Behavior

Implementation of [Last Input Priority](https://www.hitboxarcade.com/blogs/support/what-is-socd) key press favor for ZMK.

## What it does

This is a modified key press behavior that allow user to define a set keycodes for opposing cardinal directions. While a key is just actuated and any other key is not released yet, this behavior injects a release event of the holding keycode before the last actuated keystroke. And the release event that injected in advance will be ignored later.

## Installation

Include this project on your ZMK's west manifest in `config/west.yml`:

```diff
  [...]
  remotes:
+    - name: badjeff
+      url-base: https://github.com/badjeff
  projects:
+    - name: zmk-behavior-key-press-lip
+      remote: badjeff
+      revision: main
  [...]
```

Now, update your `shield.keymap` adding the behaviors.

```c
/{
        behaviors {
              
              /* Setup left and right of WASD */
              kpad: key_lip_a_d {
                      compatible = "zmk,behavior-key-press-lip";
                      #binding-cells = <1>;
                      keycodes = <A D>;
              };

              /* Setup up and down of WASD */
              kpws: key_lip_w_s {
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
