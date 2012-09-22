<?php

/**
 * @file
 * Page preprocessing.
 */

/**
 * Preprocess page display.
 */
function kickstart_ufy_alpha_preprocess_page(&$vars) {
  if ($vars['is_front']) {
    unset($vars['page']['content']['content']['content']['system_main']);
  }
}
