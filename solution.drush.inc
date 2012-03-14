<?php


/**
 * Implementation of hook_drush_command().
 *
 * @See drush_parse_command() for a list of recognized keys.
 *
 * @return
 *   An associative array describing your command(s).
 */
function solution_drush_command() {
  $items = array();

  $items['solution_install'] = array(
    'description' => "Install solution",
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_ROOT,
  );

  $items['solution-nodeaccess-rebuild'] = array(
    'description' => "Rebuild nodeaccess",
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
  );

  $items['imagecache-presets-revert-all'] = array(
    'description' => "Revert all image cache presets",
    'arguments' => array(
    ),
  );

  return $items;
}

function drush_solution_install() {
  drush_print(dt('------------------------------- Start update process'));

  if (!module_exists('drush_migrate')) {
    drush_print(dt('------------------------------- Enable drake module - START'));
    drush_invoke_process('@self', 'pm-enable', array('drush_migrate'), array('#integrate' => TRUE), TRUE);
    drush_print(dt('------------------------------- Enable drake module - FINISH'));
    drush_invoke_process('@self', 'cc', array('all'), array('#integrate' => TRUE), TRUE);
  }

  drush_print(dt('------------------------------- Run drake migrate script - START'));
  drush_invoke_process('@self', 'drake', array('--yes'), array('#integrate' => TRUE), TRUE);
  module_list(TRUE, FALSE);
  drush_print(dt('------------------------------- Run drake migrate script - FINISH'));

  drush_print(dt('------------------------------- Clear cache'));
  drush_invoke_process('@self', 'cc', array('all'), array('#integrate' => TRUE), TRUE);

  drush_print(dt('------------------------------- Run module updates - START'));
  drush_invoke_process('@self', 'updatedb', array('--yes'), array('#integrate' => TRUE), TRUE);
  drush_print(dt('------------------------------- Run module updates - FINISH'));

  drush_print(dt('------------------------------- Run nodeaccess rebuild - START'));
  drush_invoke_process('@self', 'solution-nodeaccess-rebuild', array('--yes'), array('#integrate' => TRUE), TRUE);
  drush_print(dt('------------------------------- Run nodeaccess rebuild - FINISH'));

  if (module_exists('l10n_update')) {
    drush_print(dt('------------------------------- Revert translations - START'));
    drush_invoke_process('@self', 'l10n-update-refresh', array(), array('#integrate' => TRUE));
    drush_invoke_process('@self', 'l10n-update', array(), array('#integrate' => TRUE));
    drush_print(dt('------------------------------- Revert translations - FINISH'));
  }

  if (module_exists('features')) {
    drush_print(dt('------------------------------- Revert features - START'));
    drush_invoke_process('@self', 'features-revert-all', array('--yes'), array('#integrate' => TRUE));
    drush_print(dt('------------------------------- Revert features - FINISH'));
  }

  if (module_exists('strongarm')) {
    drush_print(dt('------------------------------- Revert variables - START'));
    drush_invoke_process('@self', 'strongarm-revert', array(), array('#integrate' => TRUE));
    drush_print(dt('------------------------------- Revert variables - FINISH'));
  }

//@TODO: add hook for custom calles
  
  if (module_exists('views')) {
    drush_print(dt('------------------------------- Revert views - START'));
    drush_invoke_process('@self', 'views-revert', array('all'), array('#integrate' => TRUE));
    drush_print(dt('------------------------------- Revert views - FINISH'));
  }

  if (module_exists('panels')) {
    drush_print(dt('------------------------------- Revert panels pages - START'));
    drush_invoke_process('@self', 'panels-pages-delete-all', array(), array('#integrate' => TRUE));
    drush_print(dt('------------------------------- Revert panels pages - FINISH'));

    drush_print(dt('------------------------------- Revert panels variants - START'));
    drush_invoke_process('@self', 'panels-variants-revert-all', array(), array('#integrate' => TRUE));
    drush_print(dt('------------------------------- Revert panels variants - FINISH'));

    drush_print(dt('------------------------------- Revert panels custom content - START'));
    drush_invoke_process('@self', 'panels-custom_content-revert-all', array(), array('#integrate' => TRUE));
    drush_print(dt('------------------------------- Revert panels custom content - FINISH'));

    drush_print(dt('------------------------------- Revert panels layouts - START'));
    drush_invoke_process('@self', 'panels-layouts-revert-all', array(), array('#integrate' => TRUE));
    drush_print(dt('------------------------------- Revert panels layouts - FINISH'));
  }

  if (module_exists('imagecache')) {
    drush_print(dt('------------------------------- Revert imagecache presets - START'));
    drush_invoke_process('@self', 'imagecache-presets-revert-all', array(), array('#integrate' => TRUE));
    drush_print(dt('------------------------------- Revert imagecache presets - FINISH'));
  }

  if (module_exists('webforms')) {
    drush_print(dt('------------------------------- Revert webforms - START'));
    drush_invoke_process('@self', 'webforms-revert-all', array(), array('#integrate' => TRUE));
    drush_print(dt('------------------------------- Revert webforms - FINISH'));
  }

  if (module_exists('ubercart')) {
    drush_print(dt('------------------------------- Revert conditional actions - START'));
    drush_invoke_process('@self', 'conditional-actions-revert-all', array(), array('#integrate' => TRUE));
    drush_print(dt('------------------------------- Revert conditional actions - FINISH'));
  }

  drush_print(dt('------------------------------- Clear cache'));
  drush_invoke_process('@self', 'cc', array('all'), array('#integrate' => TRUE));

  if (module_exists('features')) {
    // we need this because some features do not revert from first run (event races problem)
    drush_print(dt('------------------------------- Revert features CONTROL - START'));
    drush_invoke_process('@self', 'features-revert-all', array('--yes'), array('#integrate' => TRUE));
    drush_print(dt('------------------------------- Revert features CONTROL - FINISH'));

    drush_print(dt('------------------------------- Clear cache'));
    drush_invoke_process('@self', 'cc', array('all'), array('#integrate' => TRUE));
  }
}


function drush_solution_nodeaccess_rebuild() {
  module_load_include('inc', 'node', 'node.admin');
  node_access_rebuild(FALSE);
}

function drush_solution_imagecache_presets_revert_all() {
  $items = imagecache_presets(TRUE);
  foreach ($items as $item) {
    if ($item['storage'] == IMAGECACHE_STORAGE_OVERRIDE) {
      imagecache_preset_delete($item);
      drush_log(dt('Preset @name (ID: @id) was deleted.', array('@name' => $item['presetname'], '@id' => $item['presetid'])), 'ok');
    }
  }
  drush_log(dt('Preset reverted.'), 'ok');
}




/*  $items['panels-pages-revert-all'] = array(
    'description' => "Revert all panels pages",
    'arguments' => array(
    ),
  );

  $items['panels-pages-delete-all'] = array(
    'description' => "Delete all panels pages",
    'arguments' => array(
    ),
  );

  $items['panels-variants-revert-all'] = array(
    'description' => "Revert all panels variants",
    'arguments' => array(
    ),
  );


  $items['panels-custom_content-revert-all'] = array(
    'description' => "Revert all panels custom content",
    'arguments' => array(
    ),
  );

  $items['panels-layouts-revert-all'] = array(
    'description' => "Revert all panels layouts",
    'arguments' => array(
    ),
  );

  $items['webforms-revert-all'] = array(
    'description' => "Revert all components of all webform",
    'arguments' => array(),
  );

  $items['pdf-template-symlink'] = array(
    'description' => "Create symbolic link for pdf template",
    'arguments' => array(),
  );

  $items['conditional-actions-revert-all'] = array(
    'description' => "Revert all Ubercart conditional actions, and disable unwanted ones.",
    'arguments' => array(),
  );

  return $items;
}

/**
 * Drush command callback that shows the listing of changed/unchanged projects.
 */
/*function drush_uni_ecommerce_panels_pages_revert_all() {
  $tasks = page_manager_get_tasks_by_type('page');
  $pages = array();
  _page_manager_get_pages($tasks, $pages);
  foreach ($pages as &$page) {
    if (empty($page['locked'])) {
      if ($page['subtask'] && $page['subtask']->type == dt('Overridden')) {
        page_manager_page_delete($page['subtask']);
        drush_log(dt('@page reverted', array('@page' => $page['admin title'])), 'ok');
      }
      else {
        drush_log(dt('@page is allready reverted', array('@page' => $page['admin title'])), 'ok');
      }
    }
    else {
      drush_set_error("pages revert", dt('@page is locked', array('@page' => $page['admin title'])));
    }
  }
  drush_log(dt('Panels reverted.'), 'ok');
}

/**
 * Drush command callback that shows the listing of changed/unchanged projects.
 */
/*function drush_uni_ecommerce_panels_pages_delete_all() {
  $tasks = page_manager_get_tasks_by_type('page');
  $pages = array();
  _page_manager_get_pages($tasks, $pages);
  foreach ($pages as &$page) {
    if (empty($page['locked'])) {
      if ($page['subtask']) {
        page_manager_page_delete($page['subtask']);
        drush_log(dt('@page reverted', array('@page' => $page['admin title'])), 'ok');
      }
      else {
        drush_log(dt('@page is allready reverted', array('@page' => $page['admin title'])), 'ok');
      }
    }
    else {
      drush_set_error("pages revert", t('@page is locked', array('@page' => $page['admin title'])));
    }
  }
  drush_log(dt('Panel pages reverted.'), 'ok');
}


/**
 * Drush command callback that shows the listing of changed/unchanged projects.
 */
/*function drush_uni_ecommerce_panels_variants_revert_all() {
  $tasks = page_manager_get_tasks_by_type('page');
  $variants = array();
  _page_manager_get_variants($tasks, $variants);
  foreach ($variants as $key => &$variant) {
    foreach ($variant->handlers as $handler) {
      page_manager_delete_task_handler($handler);
      drush_log(dt('Revert @variant variant', array('@variant' => $key)), 'ok');
    }
  }
  drush_log(dt('Panel pages variants reverted.'), 'ok');
}

/**
 * Sort tasks into buckets based upon whether or not they have subtasks.
 */
/*function _page_manager_get_pages($tasks, &$pages, $task_id = NULL) {
  foreach ($tasks as $id => $task) {
    // If a type has subtasks, add its subtasks in its own table.
    if (!empty($task['subtasks'])) {
      _page_manager_get_pages(page_manager_get_task_subtasks($task), $pages, $task['name']);
      continue;
    }

    $pages[] = $task;
  }
}

/**
 * Sort tasks into buckets based upon whether or not they have subtasks.
 */
/*function _page_manager_get_variants($tasks, &$pages, $task_id = NULL) {
  ctools_include('context');
  foreach ($tasks as $id => $task) {
    $pages[] = page_manager_get_page_cache($task['name']);
  }
}


/**
 * Drush command callback that shows the listing of changed/unchanged projects.
 */
/*function drush_uni_ecommerce_panels_custom_content_revert_all() {
  ctools_include('export');
  $items = ctools_export_crud_load_all('ctools_custom_content');
  foreach ($items as $item) {
    ctools_export_crud_delete('ctools_custom_content', $item);
    drush_log(dt('Revert @cc custom content', array('@cc' => $item->admin_title)), 'ok');
  }
  drush_log(dt('Custom content reverted.'), 'ok');
}


/**
 * Drush command callback that shows the listing of changed/unchanged projects.
 */
/*function drush_uni_ecommerce_panels_layouts_revert_all() {
  ctools_include('export');
  $items = ctools_export_crud_load_all('panels_layout');
  foreach ($items as $item) {
    ctools_export_crud_delete('panels_layout', $item);
    drush_log(dt('Revert @layout layout', array('@layout' => $item->admin_title)), 'ok');
  }
  drush_log(dt('Layouts reverted.'), 'ok');
}

/**
 * Drush command callback that shows the listing of changed/unchanged projects.
 */
/*
/**
 * Drush command callback that reverts all webform components
 */
/*function drush_uni_ecommerce_webforms_revert_all() {
  // find all published nodes of type 'webform'
  if (module_exists('webform_share')) {
    $result = db_query("SELECT nid, title FROM {node} WHERE type = 'webform' AND status = 1");
    $webforms_export_directory_path = drupal_get_path('module', 'uni_ecommerce') . '/webforms';
    while ($nid_object = db_fetch_object($result)) {
      $nid = $nid_object->nid;
      // try to load stored code of this node's webform
      $export_path = $webforms_export_directory_path . '/webform-' . $nid . '.inc';
      if (file_exists($export_path)) {
        // get exported code into a variable
        if ($exported_code = file_get_contents($export_path)) {
          $node = node_load($nid);
          // evaluate exported code and revert webform node
          if ($node && $webform = _webform_share_eval($exported_code)) {
            webform_share_update_node($node, $webform, 0);
            drush_log('Webform "' . $node->title . '" reverted.', 'success');
          }
          else {
            drush_log('Cannot revert webform "'. $node->title . '": evaluation of exported code fails.', 'error');
          }
        }
        else {
          drush_log('Cannot revert webform "'. $node->title . '": getting content of the export file fails.', 'error');
        }
      }
      else {
        drush_log('Cannot revert webform "'. $nid_object->title . '": no exported code available.', 'warning');
      }
    }
  }
  else {
    drush_log('Module webform_share not enabled. Please enable this module in order to revert webforms.', 'warning');
  }
}

function drush_uni_ecommerce_pdf_template_symlink() {
  if (!function_exists('symlink')) {
    drush_log('Creation of symbolic links impossible by PHP in this operating system', 'error');
    exit;
  }
  $target = realpath(drupal_get_path('module', 'uni_ecommerce') . '/views_pdf_templates/uni-editions-facture-template.pdf');
  if (!file_exists($target)) {
    drush_log('file not found in: '. $target, 'error');
    exit;
  }
  $link_dir = file_create_path('views_pdf_templates');
  if (!file_check_directory($link_dir, '777')) {
    drush_log('Directory in files does not exist and it couldn\'t be created.', 'error');
    exit;
  }
  $link = realpath($link_dir) . '/uni-editions-facture-template.pdf';
  if (is_link($link)) {
    drush_log('Symlink for pdf template already exists.', 'warning');
  }
  else {
    if (symlink($target, $link)) {
      drush_log('Created symlink for pdf template.', 'success');
    }
    else {
      drush_log('Symbolic link couldn\'t be created.', 'error');
    }
  }
}

/**
* Drush command callback that reverts all Ubercart conditional actions
*/
/*function drush_uni_ecommerce_conditional_actions_revert_all() {
  // revert predicates
  if (db_query("DELETE FROM {ca_predicates}")) {
    drush_log('Reverted all conditional actions.', 'success');
  }
  else {
    drush_log('Error reverting conditional actions.', 'error');
  }
  // disable unwanted predicates
  $predicates_to_disable = array('uc_checkout_customer_notification');
  foreach ($predicates_to_disable as $predicate_name) {
    $predicate = ca_load_predicate($predicate_name);
    if ($predicate) {
      $predicate['#status'] = 0;
      ca_save_predicate($predicate);
      drush_log('Disabled conditional action "' . $predicate['#title'] . '".', 'success');
    }
    else {
      drush_log('Conditional action "' . $predicate['#title'] . '" not found.', 'error');
    }
  }
}*/