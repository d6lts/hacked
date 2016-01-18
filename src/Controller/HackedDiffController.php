<?php

/**
 * @file
 * Contains \Drupal\hacked\Controller\HackedDiffController.
 */

namespace Drupal\hacked\Controller;

use Drupal\diff\EntityComparisonBase;
use Drupal\hacked\hackedFileHasher;
use Drupal\hacked\hackedProject;

/**
 * Controller routines for hacked routes.
 */
class HackedDiffController extends EntityComparisonBase {

  /**
   * Shows a diff report for a specific file in a project.
   *
   * @param                              $project
   *   The hackedProject instance.
   *
   * @param \Drupal\hacked\hackedProject $project
   * @return array
   */
  public function hackedProjectDiff(hackedProject $project) {
    if (!\Drupal::moduleHandler()->moduleExists('diff')) {
      return [
        '#markup' => $this->t('The diff module is required to use this feature.')
      ];
    }

    $file = \Drupal::request()->get('file');
    $project->identify_project();

    // Find a better way to do this:
//    $breadcrumb = array(
//      l('Home', '<front>'),
//      l('Administer', 'admin'),
//      l('Reports', 'admin/reports'),
//      l('Hacked', 'admin/reports/hacked'),
//      l($project->title(), 'admin/reports/hacked/' . $project->name),
//    );
//    drupal_set_breadcrumb($breadcrumb);

    if ($project->file_is_diffable($file)) {
      $original_file = $project->file_get_location('remote', $file);
      $installed_file = $project->file_get_location('local', $file);

      /** @var hackedFileHasher $hasher */
      $hasher = hacked_get_file_hasher();

      $build = [
        '#theme'    => 'table',
        '#header'   => [t('Original'), '', t('Current'), ''],
        '#rows'     => $this->getRows($hasher->fetch_lines($original_file), $hasher->fetch_lines($installed_file), TRUE),
      ];

      // Add the CSS for the diff.
      $build['#attached']['library'][] = 'diff/diff.general';
      $theme = $this->config->get('general_settings.theme');
      if ($theme) {
        if ($theme == 'default') {
          $build['#attached']['library'][] = 'diff/diff.default';
        }
        elseif ($theme == 'github') {
          $build['#attached']['library'][] = 'diff/diff.github';
        }
      }
      // If the setting could not be loaded or is missing use the default theme.
      elseif ($theme == NULL) {
        $build['#attached']['library'][] = 'diff/diff.github';
      }
      return $build;
    }
    return [
      '#markup' => $this->t('Cannot hash binary file or file not found: %file', array('%file' => $file))
    ];
  }

  /**
   * Menu title callback for the hacked site report page.
   */
  public function hackedProjectDiffTitle(hackedProject $project) {
    $file = \Drupal::request()->get('file');
    return $this->t('Hacked status for @file in project @project', [
      '@project' => $project->title(),
      '@file'    => $file,
    ]);
  }

}