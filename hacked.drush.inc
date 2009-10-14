<?php

// $Id$

/**
 * @file
 *   Hacked drush command.
 *
 *   Enables drush support for the Hacked! module.
 */

/**
 * Implementation of hook_drush_command().
 *
 * @See drush_parse_command() for a list of recognized keys.
 *
 * @return
 *   An associative array describing your command(s).
 */
function hacked_drush_command() {
  $items = array();

  // the key in the $items array is the name of the command.
  $items['hacked list-projects'] = array(
    // the name of the function implementing your command.
    'callback' => 'hacked_drush_project_list',
    // a short description of your command
    'description' => "List all projects that can be analysed by Hacked! ",
    'drupal dependencies' => array('hacked'),
    //'arguments' => array(
      //'views' => 'The machine name of the views to export.',
    //),
  );

  $items['hacked details'] = array(
    // the name of the function implementing your command.
    'callback' => 'hacked_drush_project_details',
    // a short description of your command
    'description' => "Show the Hacked! report about a specific project.",
    'drupal dependencies' => array('hacked'),
    'arguments' => array(
      'project' => 'The machine name of the project to report on.',
    ),
  );

  // more commands here ...

  return $items;
}

function hacked_drush_project_details($short_name = '') {
  $project = hacked_project_load($short_name);
  if (!$project) {
    return drush_set_error('HACKED_PROJECT_NOT_FOUND', dt('Could not find project: !project', array('!project' => $short_name)));
  }

  drush_print(dt('Details for project: @name', array('@name' => $project['title'])));
  drush_print(dt('Total files: @total_files, files changed: @changed_files, deleted files: @deleted_files', array(
    '@total_files' => count($project['hacked_results']),
    '@changed_files' => $project['changed_count'],
    '@deleted_files' => $project['deleted_count'],

  )));
  drush_print('');

  drush_print(dt('Detailed results:'));
  // Sort the results:
  arsort($project['hacked_results']);

  $rows[] = array(
    dt('Status'),
    dt('File'),
  );
  $show_unchanged = drush_get_option('include-unchanged', FALSE);
  foreach ($project['hacked_results'] as $file => $status) {
    if (!$show_unchanged && $status == HACKED_STATUS_UNHACKED) {
      continue;
    }
    $row = array(
    );

    // Now add the status:
    switch ($status) {
      case HACKED_STATUS_UNHACKED:
        $row[] = dt('Unchanged');
        break;
      case HACKED_STATUS_HACKED:
        $row[] = t('Changed');
        break;
      case HACKED_STATUS_DELETED:
        $row[] = t('Deleted');
        break;
      case HACKED_STATUS_UNCHECKED:
      default:
        $row[] = t('Unchecked');
        break;
    }

    $row[] = $file;


    $rows[] = $row;
  }
  drush_print_table($rows, TRUE);


}

function hacked_drush_project_list() {


  // Go get the data:
  module_load_include('inc', 'update', 'update.report');
  if ($available = update_get_available(TRUE)) {
    module_load_include('inc', 'update', 'update.compare');
    $data = update_calculate_project_data($available);
    $data = hacked_calculate_project_data($data);
    // Now print the data using drush:
    $rows[] = array(
      dt('Project'),
      dt('Version'),
      dt('Status'),
      dt('Changed'),
      dt('Deleted'),
    );
    foreach ($data as $project) {
      $row = array(
        $project['title'],
        $project['existing_version']
      );

      // Now add the status:
      switch ($project['hacked_status']) {
        case HACKED_STATUS_UNHACKED:
          $row[] = dt('Unchanged');
          break;
        case HACKED_STATUS_HACKED:
          $row[] = t('Changed');
          break;
        case HACKED_STATUS_UNCHECKED:
        default:
          $row[] = t('Unchecked');
          break;
      }

      $row[] = $project['changed_count'];
      $row[] = $project['deleted_count'];


      $rows[] = $row;
    }
    drush_print_table($rows, TRUE);

  }


}
