//
//  TTMovieListViewController.m
//  TTPlayerExample
//
//  Created by liang on 7/9/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#import <MediaPlayer/MediaPlayer.h>

#import "TTMovieListViewController.h"
#import "TTMovieListViewModel.h"
#import "TTMovieItemViewModel.h"

@interface TTMovieListViewController () <UITableViewDelegate, UITableViewDataSource>
{
    UITableView *_tableView;
}

@property (nonatomic, strong) TTMovieListViewModel *viewModel;
@property (nonatomic, strong, readonly) UITableView *tableView;

@end

@implementation TTMovieListViewController

- (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];
    
    [self.tableView reloadData];
}
- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

#pragma mark -
#pragma mark UI

- (void)setupUI {
    self.tableView.frame = self.view.bounds;
    self.tableView.delegate = self;
    self.tableView.dataSource = self;
    [self.view addSubview:self.tableView];
}

#pragma mark -
#pragma mark ViewModel

- (void)configViewModel {
    self.viewModel = [TTMovieListViewModel new];
    [self.tableView reloadData];
}

#pragma mark -
#pragma mark UITableViewDelegate

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath {
    TTMovieItemViewModel *item = [self.viewModel itemAtIndex:indexPath.row];
    if (item == nil) {
        return 44;
    } else {
        return item.height;
    }
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    TTMovieItemViewModel *item = [self.viewModel itemAtIndex:indexPath.row];
    MPMoviePlayerViewController *playerVC = [[MPMoviePlayerViewController alloc] initWithContentURL:item.url];
    [self.navigationController pushViewController:playerVC animated:YES];
}

#pragma mark -
#pragma mark UITableViewDataSource

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return self.viewModel.total;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:@"MovieListCell"];
    if (cell == nil) {
        cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:@"MovieListCell"];
    }
    
    TTMovieItemViewModel *item = [self.viewModel itemAtIndex:indexPath.row];
    cell.textLabel.text = item.name;
    
    return cell;
}

#pragma mark -
#pragma mark setter/getter

- (UITableView *)tableView {
    if (_tableView == nil) {
        _tableView = [UITableView new];
    }
    return _tableView;
}

@end
